require 'rack/request'
require 'rack/response'
require 'json'

class ApiWebApp
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
        { "informationList" => [] }
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

        response_object = send endpoint_handler, :request_body

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
        "/api/information/banner/list/get" => :information_banner_list_get,
    }

end

