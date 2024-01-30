#!/bin/sh

set -e

exec protoc --ruby_out ../asset-downloader/protocol Octo/Proto/Database.proto
