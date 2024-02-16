#!/usr/bin/env ruby

require 'net/http'
require 'json'
require 'tempfile'

CHROME_PLATFORMS_AND_ANGLE_ARTIFACTS = {
    "linux64" => "chrome-linux64/libGLESv2.so",
    "win64" => "chrome-win64/libGLESv2.dll"
}

def get_platform_download(downloads, platform)
    download = downloads.find { |download| download.fetch("platform") == platform }
    if download.nil?
        raise "failed to find the download for #{platform}"
    end
    download.fetch("url")
end

versions = JSON.parse(Net::HTTP.get(URI("https://googlechromelabs.github.io/chrome-for-testing/last-known-good-versions-with-downloads.json")))

channel = versions.fetch("channels").fetch("Stable")
puts "Downloading Chrome #{channel.fetch("version")} (revision #{channel.fetch("revision")}) to extract ANGLE"
chrome = channel.fetch("downloads").fetch("chrome")

CHROME_PLATFORMS_AND_ANGLE_ARTIFACTS.each do |platform, artifact|
    puts "  downloading #{platform}"
    Tempfile.create(mode: File::BINARY | File::CREAT | File::TRUNC | File::RDWR) do |tmpfile|
        tmpfile.write Net::HTTP::get(URI(get_platform_download(chrome, platform)))
        tmpfile.flush

        File.open("graft/ANGLE_" + File.basename(artifact), "wb") do |outf|
            unless system("unzip", "-j", "-p", tmpfile.path, artifact, out: outf)
                raise "unzip has failed"
            end
        end
    end
end
