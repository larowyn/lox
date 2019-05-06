cmake_minimum_required(VERSION 3.14)
project(lox)

set(CMAKE_CXX_STANDARD 14)

add_executable(lox main.cpp ptb/ptb_io.cpp ptb/ptb_io.h ptb/ptb_types.h lexer.cpp lexer.h ptb/ptb_standard.cpp ptb/ptb_standard.h parser.cpp parser.h ptb/ptb_arena.cpp ptb/ptb_arena.h ptb/ptb_stack.cpp ptb/ptb_stack.h ptb/ptb_substring.cpp ptb/ptb_substring.h interpreter.cpp interpreter.h error.cpp error.h state.h)