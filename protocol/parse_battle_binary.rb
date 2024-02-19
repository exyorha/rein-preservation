#!/usr/bin/env ruby

require 'bindata'
require 'msgpack'
require 'json'

class BattleBinary < BinData::Record
    endian :little

    uint8 :success_flag
    uint32 :version, assert: 5
    uint32 :checksum
    rest :messagepack_encoded_content
end

binary = BattleBinary.new
binary.read $stdin

calculated_sum = binary.messagepack_encoded_content.sum(32)
if calculated_sum != binary.checksum
    raise "bad checksum: calculated #{calculated_sum}, expected #{binary.checksum}"
end

io = StringIO.new binary.messagepack_encoded_content
u = MessagePack::Unpacker.new(io)

contents = u.read
converted = {
    "success_flag" => binary.success_flag != 0,
    "contents" => contents
}

$stdout.write JSON.generate(converted)

