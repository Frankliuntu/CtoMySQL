cmake_minimum_required(VERSION 3.25)

project(CtoMySQL)
#Please include the directory where contain mysql.h ;
include_directories(/opt/homebrew/opt/mysql/include/mysql/)

set(CMAKE_CXX_STANDARD 17)

add_executable(CtoMySQL main.cpp)

#Please also link this "libmysqlclient.dylib" for compilation;
target_link_libraries(CtoMySQL /opt/homebrew/opt/mysql/lib/libmysqlclient.dylib)