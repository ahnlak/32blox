cmake_minimum_required(VERSION 3.1)
project (32blox)
include (../../32blit.cmake)
blit_executable (32blox 32blox.cpp ball.cpp game.cpp hiscore.cpp level.cpp splash.cpp sprite.cpp 32bee_text.cpp)
