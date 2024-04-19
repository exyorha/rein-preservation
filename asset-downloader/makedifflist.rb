#!/usr/bin/env ruby

require_relative 'common'


if ARGV.size != 2
    raise "Usage: makedifflist <OLD OCTO LIST> <NEW OCTO LIST>"
end

old_list_filename, new_list_filename = ARGV

old_list = Octo::Proto::Database.decode(File.binread(old_list_filename))
new_list = Octo::Proto::Database.decode(File.binread(new_list_filename))

entries = {}

old_list.assetBundleList.each do |entry|
    if entries.include? entry.name
        raise "duplicate name: #{entry.name}"
    end

    entries[entry.name] = entry
end

old_list.resourceList.each do |entry|
    if entries.include? entry.name
        raise "duplicate name: #{entry.name}"
    end

    entries[entry.name] = entry
end

new_list.assetBundleList.each do |entry|
    old = entries[entry.name]

    if old.nil? || old.size != entry.size || old.crc != entry.crc || old.md5 != entry.md5 || old.generation != entry.generation
        puts "content/assetbundle/#{entry.name.gsub(")", "/")}.unity3d"
    end
end

new_list.resourceList.each do |entry|
    old = entries[entry.name]

    if old.nil? || old.size != entry.size || old.crc != entry.crc || old.md5 != entry.md5 || old.generation != entry.generation
        puts "content/resources/#{entry.name.gsub(")", "/")}"
    end
end
