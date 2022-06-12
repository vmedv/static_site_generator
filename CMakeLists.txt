cmake_minimum_required(VERSION 3.22)
project(static_site_generator)

set(CMAKE_CXX_STANDARD 17)

add_executable(static_site_genetator main.cpp markup.cpp markup.h gemtext.cpp gemtext.h)
