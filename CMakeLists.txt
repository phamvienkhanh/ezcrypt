cmake_minimum_required(VERSION 3.10.0)
project(chacha20 VERSION 0.1.0 LANGUAGES C)

add_executable(chacha20 chacha20.c)
add_executable(aes aes.c)

target_link_libraries(aes PRIVATE crypto)
# add_executable(chacha20 test.c)

