cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_CXX_STANDARD 20)

project( KademplaProb )

include_directories(cereal/include)

set( SRC_LIST
        main.cpp
        Swarm.h
        Node.h
        Bucket.h
        NodeStatistic.h
        Utils.h
        Constants.h)
        
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O")
add_executable( KademplaProb ${SRC_LIST} )
