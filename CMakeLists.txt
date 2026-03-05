cmake_minimum_required(VERSION 3.20)

project(EdgeRelay VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(EDGERELAY_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(EDGERELAY_ENABLE_MQTT "Enable MQTT sink using Paho MQTT C++" OFF)

add_executable(edge_relay
    src/main.cpp
    src/app_message.cpp
    src/stub_source.cpp
    src/console_sink.cpp
    src/mqtt_sink.cpp
    src/runtime_config.cpp
    src/pipeline.cpp
)

target_compile_features(edge_relay PRIVATE cxx_std_17)

target_include_directories(edge_relay PRIVATE src)

if(MSVC)
    target_compile_options(edge_relay PRIVATE /W4 /permissive-)
    if(EDGERELAY_WARNINGS_AS_ERRORS)
        target_compile_options(edge_relay PRIVATE /WX)
    endif()
else()
    target_compile_options(edge_relay PRIVATE -Wall -Wextra -Wpedantic)
    if(EDGERELAY_WARNINGS_AS_ERRORS)
        target_compile_options(edge_relay PRIVATE -Werror)
    endif()
endif()

if(EDGERELAY_ENABLE_MQTT)
    find_package(eclipse-paho-mqtt-cpp CONFIG REQUIRED)
    target_link_libraries(edge_relay PRIVATE eclipse-paho-mqtt-cpp::paho-mqttpp3)
    target_compile_definitions(edge_relay PRIVATE EDGERELAY_ENABLE_MQTT)
endif()

install(TARGETS edge_relay)
