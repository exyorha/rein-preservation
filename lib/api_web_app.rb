require 'rack/request'
require 'rack/response'
require 'json'
require 'sqlite3'

class ApiWebApp
    def initialize
        @db = SQLite3::Database.new ':memory:'
        init = File.read(File.expand_path("../../information.sql", __FILE__))
        until init.empty?
            statement = @db.prepare init
            init = statement.remainder
            init.strip!

            statement.execute

        end
    end

    def call(env)
        request = Rack::Request.new env

        response_status, response_object = process_request(request)

        response = Rack::Response.new JSON.generate(response_object), response_status

        response.content_type = "application/json"
        response.cache_control = "no-store"

        response.finish
    end

    private

    def information_list_get(request)
        typeList = request.fetch("informationTypeList")

        query = @db.prepare <<EOF
SELECT
    informationId,
    webViewMissionId,
    informationType,
    title,
    publishStartDatetime,
    postscriptDatetime
FROM information
WHERE informationType IN (#{typeList.map { "?" }.join(", ")})
ORDER BY COALESCE(postscriptDatetime, publishStartDatetime) DESC
LIMIT ? OFFSET ? - 1
EOF
        params = typeList.dup
        params.push request.fetch("limit")
        params.push request.fetch("offset")

        query.bind_params params

        informationList = []

        query.execute! do |(information_id, web_view_mission_id, information_type, title, publish_start_datetime, postscript_datetime)|
            item = {
                "informationId" => information_id,
                "webViewMissionId" => web_view_mission_id,
                "informationType" => information_type,
                "title" => title,
                "publishStartDatetime" => publish_start_datetime
            }

            unless postscript_datetime.nil?
                item["postscriptDatetime"] = postscript_datetime
            end

            informationList.push(item)
        end

        { "informationList" => informationList }
    end

    def information_detail_get(request)
        query = @db.prepare "SELECT informationType, title, body, publishStartDatetime, postscriptDatetime FROM information WHERE informationId = ?"
        query.bind_param 1, request.fetch("informationId")

        query.execute! do |(information_type, title, body, publish_start_datetime, postscript_datetime)|
            item = {
                "informationType" => information_type,
                "title" => title,
                "body" => body,
                "publishStartDatetime" => publish_start_datetime
            }

            unless postscript_datetime.nil?
                item["postscriptDatetime"] = postscript_datetime
            end

            return item
        end

        raise "no such information item"
    end

    def information_banner_list_get(request)
        { "bannerList" => [] }
    end

    def process_request(request)
        if not request.post?
            return [ 405, {} ]
        end

        request_body = JSON.parse request.body.read

        endpoint_handler = ENDPOINTS[request.path_info]
        if endpoint_handler.nil?
            return [ 404, {} ]
        end

        response_object = send endpoint_handler, request_body

        add_common_response response_object

        return [ 200, response_object ]

    rescue => e
        warn "an exception has occurred while processing the request: #{e.inspect}"

        return [ 500, {} ]
    end

    def add_common_response(response)
        response["commonResponse"] = { "responseDatetime" => (Time.now.to_f * 1000).round }
        nil
    end

    ENDPOINTS = {
        "/api/information/list/get" => :information_list_get,
        "/api/information/detail/get" => :information_detail_get,

        "/api/information/banner/list/get" => :information_banner_list_get,
    }

end

