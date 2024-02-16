#!/usr/bin/env ruby

require_relative 'common'

downloader = create_default_downloader()

database = downloader.load_list(!ARGV.empty?)

puts "Loaded the database of the revision level #{database.revision}, containing #{database.assetBundleList.size} assets and #{database.resourceList.size} resources."

if downloader.up_to_date?
    puts "the asset list is up to date."
end

downloader.uri_format = database.urlFormat

database.assetBundleList.each_with_index do |asset, index|
#    next if index < first_index

    path = downloader.acquire_file asset, 'assetbundle'
end

database.resourceList.each_with_index do |asset, index|
    path = downloader.acquire_file asset, 'resources'
end
