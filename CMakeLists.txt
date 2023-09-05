cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_CXX_STANDARD 20)

project( KademplaProb )

set( SRC_LIST main.cpp Swarm.h Node.h Bucket.h NodeStatistic.h Utils.h )

add_executable( KademplaProb ${SRC_LIST} )
