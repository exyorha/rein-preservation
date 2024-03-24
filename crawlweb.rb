#!/usr/bin/env ruby

require 'bundler/setup'
require 'json'
require 'curb'
require 'fileutils'

def call_api(auth, url, request = {})
    request = JSON.generate(request.merge({
        "commonRequest" => {
            "requestDatetime" => (Time.now.to_f*1000).round
        }.merge(auth)
    }))

    puts "requesting: #{url}, #{request}"

    res = Curl.post(url, request) do |http|
        http.headers["Accept"] = "application/json, text/plain, */*"
        http.headers["Content-Type"] = "application/json"
        http.headers["User-Agent"] = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36"
    end

    puts "response: #{res.code}"

    response = JSON.parse(res.body)

    if res.code != 200
        raise "API call failed: #{response.inspect}"
    end

    response

end

FALLBACK_AUTH = {
    "appVersion" => "0.1",
    "language" => "Japanese",
    "osType" => 1,
    "osVersion" => "Mac OS X 10.14.3",
    "deviceName" => "MacBookPro15,1",
    "requestId" => 1,
    "platformType" => 1
}

auth =
    begin
        JSON.parse(File.read("auth.json"))
    rescue
        FALLBACK_AUTH
    end

FileUtils.mkpath 'information/list'
FileUtils.mkpath 'information/detail'
FileUtils.mkpath 'information/banner'
FileUtils.mkpath 'panel_mission'

(1..9).each do |mission_id|
    puts "retrieving panel mission #{mission_id}"

    result = call_api(auth, "https://api-web.app.nierreincarnation.com/api/wm/panel_mission/pages", {
        "webviewMissionId" => mission_id
    })

    File.write "panel_mission/#{mission_id}.json", JSON.generate(result)
end

exit 0

offset = 1
limit = 10

while true

    response = call_api(auth, "https://api-web.app.nierreincarnation.com/api/information/list/get", {
        "informationTypeList" => [ 1,2,3,4,5,6,7,8,9,10,11,12 ],
        "limit" => limit,
        "offset" => offset
    })

    items = response.fetch "informationList"

    items.each do |item|
        id = item.fetch("informationId")

        puts "Got information item #{id}"

        File.write "information/list/#{id}.json", JSON.generate(item)

        thumbnail = item["thumbnailImagePath"]
        if !thumbnail.nil?
            local_thumbnail = "public/#{thumbnail}"

            unless File.exist? local_thumbnail
                FileUtils.mkpath File.dirname(local_thumbnail)
                unless system "wget", "-O", local_thumbnail, "https://web.app.nierreincarnation.com#{thumbnail}"
                    raise "failed to download the thumbnail for #{id}"
                end
            end
        end

        detail = "information/detail/#{id}.json"
        unless File.exist? detail

            detail_response = call_api(auth, "https://api-web.app.nierreincarnation.com/api/information/detail/get", { "informationId" => id })
            File.binwrite detail, JSON.generate(detail_response)
        end
    end

    if items.empty?
        break
    end

    offset += items.size
end

offset = 1
while true

    response = call_api(auth, "https://api-web.app.nierreincarnation.com/api/information/banner/list/get", {
        "informationTypeList" => [ 1,2,3,4,5,6,7,8,9,10,11,12 ],
        "limit" => limit,
        "offset" => offset
    })

    items = response.fetch "bannerList"

    p [offset, limit, items.size]

    items.each do |item|
        id = item.fetch("informationId")

        puts "Got information item #{id} banner entry"

        File.write "information/banner/#{id}.json", JSON.generate(item)
    end

    if items.empty?
        break
    end

    offset += items.size

    # It appears that this service doesn't implement pagination.
    break
end


