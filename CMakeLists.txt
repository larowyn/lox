cmake_minimum_required(VERSION 3.14)
project(lox)

set(CMAKE_CXX_STANDARD 14)

#@todo: add -Werror
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-writable-strings")

add_executable(
        lox
        main.cpp
        ptb/ptb_io.cpp
        ptb/ptb_io.h
        ptb/ptb_types.h
        lexer.cpp
        lexer.h
        ptb/ptb_standard.cpp
        ptb/ptb_standard.h
        parser.cpp
        parser.h
        ptb/ptb_arena.cpp
        ptb/ptb_arena.h
        ptb/ptb_array.cpp
        ptb/ptb_array.h
        ptb/ptb_substring.cpp
        ptb/ptb_substring.h
        interpreter.cpp
        interpreter.h
        error.cpp
        error.h
        state.h
        ptb/ptb_map.cpp ptb/ptb_map.h)