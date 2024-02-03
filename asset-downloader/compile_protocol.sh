#!/bin/sh

set -e

exec protoc --ruby_out protocol -I ../CompatibilityRuntime/GameExecutable ../CompatibilityRuntime/GameExecutable/Octo/Proto/Database.proto
