#!/usr/bin/env ruby

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

        @uri_format
    end

    def load_list
        highest_version = nil

        Dir[File.join(@lists_path, "*.pb")].each do |list_filename|
            version = Integer(File.basename(list_filename, '.pb'))

            if highest_version.nil? || version > highest_version
                highest_version = version
            end
        end

        if highest_version.nil?
            puts "No local list, downloading"
            return download_list()
        else
            puts "Using the pre-existing list of the revision level #{highest_version}"

            return Octo::Proto::Database.decode(File.binread(File.join(@lists_path, "#{highest_version}.pb")))
        end
    end

    def acquire_file(descriptor, type)
        local_path = File.join(@content_path, type, descriptor.name.gsub(")", "/"))

        if File.extname(local_path).empty? && type == 'assetbundle'
            local_path << ".unity3d"
        end

        if File.exist? local_path
            # TODO: validate existing local_path
        else
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

                    File.rename "#{local_path}.part", local_path
                end

            end
        end

        return local_path
    end

    private

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
end

downloader = OctoAssetDownloader.new({
    app_id: 301,
    client_secret_key: 'l488k2zmalogay245osa257ifw2lczq4',
    client_aes_key: 'st4q3c7p1ibgwdhm',
    octo_version: 300116832,
    base_url: 'https://resources-api.app.nierreincarnation.com/'
}, File.expand_path("../../content", __FILE__))

database = downloader.load_list

puts "Loaded the database of the revision level #{database.revision}, containing #{database.assetBundleList.size} assets and #{database.resourceList.size} resources."

downloader.uri_format = database.urlFormat

database.assetBundleList.each_with_index do |asset, index|
    puts "#{(index * 100.0 / database.assetBundleList.size).round(2)}% processed"
    path = downloader.acquire_file asset, 'assetbundle'
end
