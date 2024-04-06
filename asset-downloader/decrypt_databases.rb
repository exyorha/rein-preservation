#!/usr/bin/env ruby

require 'openssl'


Dir[File.expand_path("../../content/database/**/*.bin.e", __FILE__)].each do |encrypted_filename|
    unencrypted_filename = encrypted_filename[0..-3]

    puts "#{encrypted_filename} -> #{unencrypted_filename}"

    encrypted_data = File.binread encrypted_filename

    aes = OpenSSL::Cipher.new 'AES-128-CBC'
    aes.decrypt
    aes.iv = "EfcAef4CAe5f6DaA"
    aes.key = "6Cb01321EE5e6bBe"

    decrypted_data = aes.update(encrypted_data)
    decrypted_data << aes.final

    File.binwrite unencrypted_filename, decrypted_data
end
