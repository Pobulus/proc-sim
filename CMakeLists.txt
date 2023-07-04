cmake_minimum_required(VERSION 3.18)

project(maszynaw)

include(FetchContent)

Fetchcontent_Declare(SFML
	GIT_REPOSITORY https://github.com/SFML/SFML.git
	GIT_TAG 2.6.x)
Fetchcontent_MakeAvailable(SFML)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

find_package(X11 REQUIRED)

message(STATUS "X11_FOUND = ${X11_FOUND}")
message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
message(STATUS "X11_LIBRARIES= ${X11_LIBRARIES}")

include_directories(${X11_INCLUDE_DIR})

link_directories(${X11_LIBRARIES})


add_executable(maszynaw src/main.cpp)


target_link_libraries(maszynaw PRIVATE sfml-graphics nlohmann_json::nlohmann_json ${X11_LIBRARIES})

target_compile_features(maszynaw PRIVATE cxx_std_17)

add_custom_command(TARGET maszynaw POST_BUILD COMMAND notify-send "Compiled!" "Your binary is ready :)")

install(TARGETS maszynaw RUNTIME DESTINATION bin)
