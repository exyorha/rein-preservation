#!/usr/bin/env ruby

require 'msgpack'
require 'lz4-ruby'
require 'stringio'
require 'json'

if ARGV.size != 2
    warn "usage: parse_database <DATABASE.BIN> <OUTPUT JSON FILE>"
    exit 1
end

def extension_block_lz4(data)

    data_contents = StringIO.new(data)
    data_unpacker = MessagePack::Unpacker.new(data_contents)

    uncompressed_length = data_unpacker.read

    compressed_start = data_contents.pos

    compressed = data_unpacker.buffer.read_all
    compressed << data_contents.read

    uncompressed_data, uncompressed_data_length = LZ4::Raw.decompress(compressed, uncompressed_length)

    if uncompressed_data_length != uncompressed_length
        raise "the actual length of the uncompressed data didn't match the expected length"
    end

    uncompressed_contents = StringIO.new(uncompressed_data)
    uncompressed_unpacker = MessagePack::Unpacker.new(uncompressed_contents)
    uncompressed_unpacker.register_type(0x63) { |data| extension_block_lz4(data) }

    return uncompressed_unpacker.read
end

recomposed_document = {}

File.open(ARGV[0], "rb") do |io|
    u = MessagePack::Unpacker.new(io)
    u.register_type(0x63) { |data| extension_block_lz4(data) }

    table_of_tables = u.read

    start_of_tables = io.pos - u.buffer.size


    table_of_tables.each do |table_name, (offset, length)|
        puts "table #{table_name}, offset #{offset}, length #{length}"

        table_contents = []

        io.seek start_of_tables + offset
        table_raw_data = io.read(length)

        p table_raw_data.size

        table_stream = StringIO.new(table_raw_data)
        u = MessagePack::Unpacker.new(table_stream)
        u.register_type(0x63) { |data| extension_block_lz4(data) }

        u.each do |object|
            table_contents.push object
        end

        recomposed_document[table_name] = table_contents
    end
end

File.open(ARGV[1], 'w') do |outf|
    outf.write JSON.pretty_generate(recomposed_document)
end
