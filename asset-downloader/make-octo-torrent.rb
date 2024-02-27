#!/usr/bin/env ruby

require_relative 'common'
require 'bencode'
require 'openssl'


class TorrentWriter
    def initialize(name, piece_length = 256 * 1024)
        @piece_length = piece_length
        @pieces = "".b
        @files = []
        @partial_piece_length = 0
        @hash = OpenSSL::Digest::SHA1.new

        @torrent_info = {
            "info".b => {
                "name".b => name.encode('UTF-8').b,
                "piece length".b => @piece_length,
                "pieces".b => @pieces,
                "files".b => @files
            }
        }
    end

    def encode
        finish_piece

        @torrent_info.bencode
    end

    def add_file(filename, size)
        @files.push({
            "length".b => size,
            "path".b => filename.dup.b.split("/".b)
        })
    end

    def add_file_contents(io)
        while true
            length_to_read = @piece_length - @partial_piece_length

            data = io.read length_to_read
            if data.nil?
                break
            end

            @hash.update data
            @partial_piece_length += data.size

            if @partial_piece_length == @piece_length
                finish_piece
            end
        end
    end

    private

    def finish_piece
        if @partial_piece_length != 0
            @pieces << @hash.digest.b
        end

        @partial_piece_length = 0
        @hash = OpenSSL::Digest::SHA1.new
    end
end

downloader = create_default_downloader()

database = downloader.load_list()

downloader.uri_format = database.urlFormat

puts "Writing a torrent file for the database of version #{database.revision}"

torrent = TorrentWriter.new "octo#{database.revision}", 8 * 1024 * 1024


database.assetBundleList.each_with_index do |asset, index|
    storage_name = asset.storage_file_name('assetbundle')

    torrent.add_file storage_name, asset.size

    path = downloader.acquire_file asset, 'assetbundle'

    File.open(path, 'rb') do |inf|
        torrent.add_file_contents inf
    end
end

database.resourceList.each_with_index do |asset, index|
#    path = downloader.acquire_file asset, 'resources'

    storage_name = asset.storage_file_name('resources')

    torrent.add_file storage_name, asset.size

    path = downloader.acquire_file asset, 'resources'

    File.open(path, 'rb') do |inf|
        torrent.add_file_contents inf
    end
end

File.binwrite "octo#{database.revision}.torrent", torrent.encode
