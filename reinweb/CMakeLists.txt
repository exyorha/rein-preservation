cmake_minimum_required(VERSION 3.20)
project(reinweb)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/public/ DESTINATION GameServer/WebRoot COMPONENT GameServer)
