require 'bundler/setup'
require 'sqlite3'
require 'json'
require 'fileutils'
require 'openssl'

CARD_FULL = OpenSSL::Digest::SHA256.hexdigest("card_full.jpg")

def ensure_download_asset(path)
    puts "downloading asset: #{path}"

    local_path = "public#{path}"

    unless File.exist? local_path
        FileUtils.mkpath File.dirname(local_path)
        unless system "wget", "-O", local_path, "https://web.app.nierreincarnation.com#{path}"
            raise "failed to download the thumbnail for #{id}"
        end
    end
end

db = SQLite3::Database.new 'information.db'
db.execute <<EOF
CREATE TABLE IF NOT EXISTS information (
    informationId INTEGER NOT NULL PRIMARY KEY,
    webviewMissionId INTEGER NOT NULL DEFAULT 0,
    informationType INTEGER NOT NULL,
    title TEXT NOT NULL,
    publishStartDatetime INTEGER NOT NULL,
    body TEXT NOT NULL,
    postscriptDatetime INTEGER,
    bannerImagePath TEXT,
    thumbnailImagePath TEXT
)
EOF

statement = db.prepare(<<EOF
INSERT OR REPLACE INTO information (
    informationId,
    webviewMissionId,
    informationType,
    title,
    publishStartDatetime,
    body,
    postscriptDatetime,
    bannerImagePath,
    thumbnailImagePath
) VALUES (
    ?, ?, ?, ?, ?, ?, ?, ?, ?
)
EOF
)

db.transaction do

    Dir["information/list/*.json"].each do |filename|
        basename = File.basename filename
        content_filename = File.join("information/detail", basename)

        list_item = JSON.parse File.binread(filename)
        content = JSON.parse File.binread(content_filename)

        information_type = list_item.fetch("informationType")
        if information_type != content.fetch("informationType")
            raise "inconsistent informationType"
        end

        title = list_item.fetch("title")
        if title != content.fetch("title")
            raise "inconsistent title"
        end

        start_datetime = list_item.fetch("publishStartDatetime")
        if start_datetime != content.fetch("publishStartDatetime")
            raise "inconsistent publishStartDatetime"
        end

        postscript_datetime = list_item.fetch("postscriptDatetime", nil)
        if postscript_datetime != content.fetch("postscriptDatetime", nil)
            raise "inconsistent postscriptDatetime"
        end

        body = content.fetch("body")

        id =  list_item.fetch("informationId")

        puts "processing: #{id}"

        banner_image_path = nil

        begin
            banner_data = JSON.parse File.binread(File.join("information/banner", basename))

            if banner_data.fetch("informationId") != id
                raise "inconsistent informationId in the banner entry"
            end

            if banner_data.fetch("webviewMissionId") != list_item.fetch("webviewMissionId")
                raise "inconsistent webviewMissionId in the banner entry"
            end

            if banner_data.fetch("informationType") != information_type
                raise "inconsistent informationType in the banner entry"
            end

            banner_image_path = banner_data.fetch("bannerImagePath")

            ensure_download_asset banner_image_path
        rescue => e
            puts "failed to get the banner image for #{id}: #{e.inspect}"
        end

        thumbnail_image_path = list_item.fetch("thumbnailImagePath", nil)
        unless thumbnail_image_path.nil?
            ensure_download_asset thumbnail_image_path
        end

        statement.bind_param(1, id)
        statement.bind_param(2, list_item.fetch("webviewMissionId"))
        statement.bind_param(3, information_type)
        statement.bind_param(4, title)
        statement.bind_param(5, start_datetime)
        statement.bind_param(6, content.fetch("body"))
        statement.bind_param(7, postscript_datetime)
        statement.bind_param(8, banner_image_path)
        statement.bind_param(9, thumbnail_image_path)
        statement.execute!
        statement.reset!

        body.scan /img src=\"([^\"]+)\"/ do
            ensure_download_asset $1
        end
    end

    Dir["panel_mission/*.json"].each do |mission_file|
       mission = JSON.parse(File.read(mission_file)).fetch("data")

       mission.fetch("pages").each do |page|
            page.fetch("panels").each do |panel|
                panel.fetch("reward").fetch("rewardImageFileName")
            end

            directory = "/images/en/operations/panel_mission/#{mission.fetch("webviewPanelMissionId")}/page#{page.fetch("pageNumber")}/card"

            page.fetch("bgParts").each do |bgPart|
                ensure_download_asset "#{directory}/#{bgPart.fetch("imagePath")}"
            end

            ensure_download_asset "#{directory}/#{CARD_FULL}"

            unless system "jpegtran", "-outfile", "#{page.fetch("pageId")}.jpg", "-rotate", "90", "public#{directory}/#{CARD_FULL}"
                raise "jpegtran has failed"
            end
       end
    end
end
