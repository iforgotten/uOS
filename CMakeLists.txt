cmake_minimum_required(VERSION 3.9)
project(uOS)

set(CMAKE_CXX_STANDARD 11)

add_executable(uOS
        boot/bootasm.h
        boot/bootasm.S
        boot/bootmain.c
        libs/defs.h
        libs/x86.h
        tools/sign.c libs/elf.h)
