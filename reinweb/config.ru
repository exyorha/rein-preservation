require 'bundler/setup'
require 'rack/static'

require_relative 'lib/api_web_app'

class WebFallbackServer < Rack::Files
    def call(env)
        env["PATH_INFO"] = "/default_page.html"

        super(env)
    end
end

map "/api-web.app.nierreincarnation.com" do
    run ApiWebApp.new
end

use Rack::Static, urls: [""], root: File.expand_path("../public", __FILE__), cascade: true

run WebFallbackServer.new(File.expand_path("../public", __FILE__))

