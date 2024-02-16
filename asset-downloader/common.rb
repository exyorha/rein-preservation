
require 'uri'
require 'net/http'
require 'openssl'
require 'pry'
require 'fileutils'

require_relative 'protocol/Octo/Proto/Database_pb.rb'

class HttpConnectionPool
    ConnectionKey = Struct.new :host, :port, :use_tls

    def initialize
        @connections = Hash.new do |hash, key|
            conn = Net::HTTP.new(key.host, key.port)
            conn.use_ssl = key.use_tls

            hash[key] = conn

            conn.start

            conn
        end

    end

    def parse_uri_and_get_connection(uri)
        if not uri.kind_of? URI
            uri = URI.parse uri
        end

        port = nil
        use_tls = false

        case uri.scheme
        when 'http'
            use_tls = false
            port = Net::HTTP.http_default_port

        when 'https'
            use_tls = true
            port = Net::HTTP.https_default_port

        else
            raise "unsupported request scheme: #{port}"
        end

        unless uri.port.nil?
            port = uri.port
        end

        host = uri.host
        if host.nil?
            raise "no host in the URI"
        end

        key = ConnectionKey.new(host, port, use_tls)
        connection = @connections[key]

        return connection, uri.request_uri
    end
end

class OctoAssetDownloader
    attr_accessor :uri_format

    def initialize(params, content_path)
        @app_id = params.fetch :app_id
        @client_secret_key = params.fetch :client_secret_key
        @client_aes_key = params.fetch :client_aes_key
        @octo_version = params.fetch :octo_version
        @base_url = params.fetch :base_url

        @pool = HttpConnectionPool.new

        @content_path = content_path
        @lists_path = File.join(@content_path, "lists")

        FileUtils.mkpath @lists_path

        @up_to_date = false
    end

    def up_to_date?
        @up_to_date
    end

    def load_list(update = false)
        highest_version = nil

        Dir[File.join(@lists_path, "*.pb")].each do |list_filename|
            version = Integer(File.basename(list_filename, '.pb'))

            if highest_version.nil? || version > highest_version
                highest_version = version
            end
        end

        if highest_version.nil? || update
            list = download_list()
        else
            list = Octo::Proto::Database.decode(File.binread(File.join(@lists_path, "#{highest_version}.pb")))
        end

        @up_to_date = !highest_version.nil? && list.revision == highest_version

        list
    end

    def calculate_asset_md5(inf, initial = nil)
        running_md5sum = OpenSSL::Digest::MD5.new

        unless initial.nil?
            running_md5sum.update initial
        end

        while true
            chunk = inf.read(128*1024)
            break if chunk.nil? || chunk.empty?

            running_md5sum.update(chunk)
        end

        running_md5sum.hexdigest
    end

    def validate_asset(local_path, descriptor, is_assetbundle = false)
        md5 = nil

        File.open(local_path, "rb") do |inf|
            if inf.size != descriptor.size
                warn "Asset size doesn't match the descriptor: #{local_path}, #{descriptor.size} in the descriptor, #{inf.size} actual"
            end

            if is_assetbundle
                header = inf.read(256)
                if header[0] == '1' || header[0] == '2'
                    # The asset is masked, so it can be verified directly.
                    md5 = calculate_asset_md5 inf, header
                else
                    # The asset is not masked, and we need to re-mask it to validate.
                    masking_bytes = generate_masking_bytes(descriptor.name)

                    # We check mode 1 first (256 first bytes only), since it's
                    # used for all but the smallest assets, and we waste the
                    # least amount of CPU time that way.

                    masked_header = do_unmask(header, masking_bytes)
                    masked_header[0] = '1'
                    md5 = calculate_asset_md5(inf, masked_header)

                    if md5 != descriptor.md5
                        # Was this asset shipped plaintext?
                        inf.rewind
                        md5 = calculate_asset_md5 inf
                    end

                    if md5 != descriptor.md5
                        # Still no. Try masking the whole file.

                        inf.seek header.size
                        header << inf.read

                        masked = do_unmask(header, masking_bytes)
                        masked[0] = '2'

                        md5 = OpenSSL::Digest::MD5.hexdigest(masked)
                    end
                end
            else
                md5 = calculate_asset_md5(inf)
            end

        end

        matches = md5 == descriptor.md5

        unless matches
            warn "Asset MD5 doesn't match the descriptor: #{local_path}, #{md5} calculated, #{descriptor.md5} in the descriptor"
        end

        return matches, md5
    end

    def acquire_file(descriptor, type)
        local_path = File.join(@content_path, type, descriptor.name.gsub(")", "/"))

        if File.extname(local_path).empty? && type == 'assetbundle'
            local_path << ".unity3d"
        end

        if File.exist? local_path
            result, actual_md5 = validate_asset(local_path, descriptor, type == 'assetbundle')

            if not result
                File.rename local_path, "#{local_path}.old.#{actual_md5}"
            end
        end

        if !File.exist?(local_path)
            puts "Downloading #{descriptor.name}"

            uri = dotnet_style_format(@uri_format, {
                "v" => descriptor.uploadVersionId,
                "type" => type,
                "o" => descriptor.objectName,
                "g" => descriptor.generation
            })

            FileUtils.mkpath File.dirname(local_path)

            http, path = @pool.parse_uri_and_get_connection(uri)

            File.open("#{local_path}.part", "wb+") do |partf|
                http.request_get(path, {
                    "User-Agent" => "Dalvik/2.1.0 (Linux; U; Android 14; sdk_gphone64_x86_64 Build/UE1A.230829.036.A1)"
                }) do |resp|
                    unless resp.kind_of? Net::HTTPSuccess
                        raise "Failed to retrieve the asset from #{uri}: #{resp.inspect}"
                    end

                    running_size = 0
                    running_md5sum = OpenSSL::Digest::MD5.new

                    resp.read_body do |chunk|
                        running_size += chunk.size
                        running_md5sum.update(chunk)

                        partf.write chunk
                    end

                    md5 = running_md5sum.hexdigest
                    if running_size != descriptor.size || md5 != descriptor.md5
                        raise "Calculated integrity parameters didn't match the descriptor: #{descriptor.inspect}; size received: #{running_size}, MD5 received: #{md5}"
                    end
                end

            end

            File.rename "#{local_path}.part", local_path

            set_modtime_from_descriptor local_path, descriptor
        end

        if type == 'assetbundle'
            unmask_asset_if_needed local_path, descriptor
        end

        return local_path
    end

    private

    def set_modtime_from_descriptor(local_path, descriptor)
        time = Time.at(descriptor.generation / 1.0e6)
        File.lutime time, time, local_path
    end

    def dotnet_style_format(fmt, replacements = {})
        fmt.gsub(/\{(.+?)\}/) do |match|
            replacements.fetch($1)
        end
    end

    def download_list(current_version = 0)
        list_uri = "#{@base_url}v2/pub/a/#{@app_id}/v/#{@octo_version}/list/#{current_version}"

        http, path = @pool.parse_uri_and_get_connection(list_uri)

        req = http.request_get(path, {
            "User-Agent" => "UnityPlayer/2019.4.29f1 (UnityWebRequest/1.0, libcurl/7.52.0-DEV)",
            "Accept" => "application/x-protobuf,x-octo-app/#{@app_id}",
            "X-OCTO-KEY" => @client_secret_key,
            "X-Unity-Version" => "2019.4.29f1"
        })

        unless req.kind_of? Net::HTTPSuccess
            raise "Failed to retrieve the asset list from #{list_uri}: #{req.inspect}"
        end

        data = decrypt_list(req.body)

        database = Octo::Proto::Database.decode(data)

        puts "Retrieved Octo file list of the version #{database.revision}"

        File.binwrite(File.join(@lists_path, "#{database.revision}.pb"), data)

        database
    end

    def f_value
        "dark_#{@app_id}_#{@octo_version}"
    end

    def decrypt_list(data)
        aes = OpenSSL::Cipher.new('AES-256-CBC')
        aes.decrypt
        aes.key = OpenSSL::Digest::SHA256.digest(f_value)
        aes.iv = data.slice! 0, 16

        decrypted = aes.update(data)
        decrypted << aes.final

        decrypted
    end

    def unmask_asset_if_needed(localpath, descriptor)
        File.open(localpath, 'rb') do |inf|
            header = inf.read(256);

            if header[0] == 'U'
                # Already unmasked
                return
            end

            masking_bytes = generate_masking_bytes(descriptor.name)

            # The file needs unmasking.
            File.open(localpath + ".unmasked", "wb") do |outf|
                if header[0] == '1'
                    # In mode 1, only the first 256 bytes are masked. We already have read them

                elsif header[0] == '2'
                    # In mode 2, the whole file is masked, but the file is generally short.

                    header << inf.read

                else
                    raise "unknown masking scheme, the first few bytes are #{header[0...16].inspect}"
                end

                unmasked_header = do_unmask(header, masking_bytes)
                unmasked_header[0] = 'U'
                unless unmasked_header.start_with? "UnityFS"
                    raise "#{localpath} didn't seem to unmask correctly"
                end

                outf.write unmasked_header

                # Now, copy the plaintext tail, if any exists.
                IO.copy_stream inf, outf
            end

            validates, md5 = validate_asset(localpath + ".unmasked", descriptor, true)
            unless validates
                raise "unmasked #{localpath} doesn't validate"
            end

            File.rename(localpath + ".unmasked", localpath)

            set_modtime_from_descriptor(localpath, descriptor)

            puts "#{localpath} has been unmasked"
        end
    end

    def generate_masking_bytes(string)
        string_utf16_codepoints = string.encode("UTF-16LE").unpack("v*")

        mask_length = string_utf16_codepoints.size * 2

        i = 0
        j = mask_length - 1

        mask_bytes = Array.new(mask_length)

        string_utf16_codepoints.each do |codepoint|
            byte = codepoint & 0xFF

            mask_bytes[i] = byte
            mask_bytes[j] = byte ^ 0xFF

            i += 2
            j -= 2
        end

        mask_of_mask = 0xBB
        mask_bytes.each do |byte|
            mask_of_mask = (((mask_of_mask & 1) << 7) | (mask_of_mask >> 1)) ^ byte
        end

        mask_bytes.map! do |byte|
            byte ^ mask_of_mask
        end

        mask_bytes
    end

    def do_unmask(data, bytes)
        content = data.unpack("C*")

        Array.new(content.size) do |index|
            content[index] ^ bytes[index % bytes.size]
        end.pack("C*")
    end
end

def create_default_downloader
    OctoAssetDownloader.new({
        app_id: 301,
        client_secret_key: 'l488k2zmalogay245osa257ifw2lczq4',
        client_aes_key: 'st4q3c7p1ibgwdhm',
        octo_version: 300116832,
        base_url: 'https://resources-api.app.nierreincarnation.com/'
    }, File.expand_path("../../content", __FILE__))
end

