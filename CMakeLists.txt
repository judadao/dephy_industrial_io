cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(dephy_industrial_io)

target_sources(app PRIVATE
  src/industrial_io.c
  src/payload.c
  src/mqtt_bridge.c
)

target_include_directories(app PRIVATE include)
