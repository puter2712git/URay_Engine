cmake_minimum_required(VERSION 3.10)
project("Clicker")

add_executable("Clicker" WIN32 "main.cpp" "TestComponent.h" "TestComponent.cpp")

target_link_libraries("Clicker" PRIVATE URayEngine)