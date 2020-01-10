cmake_minimum_required(VERSION 2.8.9)
project (32blox)
include (../../32blit.cmake)
blit_executable (32blox 32blox.cpp splash.cpp sprite.cpp)
