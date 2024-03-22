require 'bundler/setup'
require 'sqlite3'
require 'json'
require 'fileutils'

db = SQLite3::Database.new 'information.db'
db.execute <<EOF
CREATE TABLE IF NOT EXISTS information (
    informationId INTEGER NOT NULL PRIMARY KEY,
    webviewMissionId INTEGER NOT NULL DEFAULT 0,
    informationType INTEGER NOT NULL,
    title TEXT NOT NULL,
    publishStartDatetime INTEGER NOT NULL,
    body TEXT NOT NULL,
    postscriptDatetime INTEGER
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
    postscriptDatetime
) VALUES (
    ?, ?, ?, ?, ?, ?, ?
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

        statement.bind_param(1, id)
        statement.bind_param(2, list_item.fetch("webviewMissionId"))
        statement.bind_param(3, information_type)
        statement.bind_param(4, title)
        statement.bind_param(5, start_datetime)
        statement.bind_param(6, content.fetch("body"))
        statement.bind_param(7, postscript_datetime)
        statement.execute!
        statement.reset!

        body.scan /img src=\"([^\"]+)\"/ do
            path = $1

            local_path = "public#{path}"

            unless File.exist? local_path
                FileUtils.mkpath File.dirname(local_path)
                unless system "wget", "-O", local_path, "https://web.app.nierreincarnation.com#{path}"
                    raise "failed to download the thumbnail for #{id}"
                end
            end

            p path
        end
    end
end
