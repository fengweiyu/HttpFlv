cmake_minimum_required (VERSION 2.8)

SET(CMAKE_TOOLCHAIN_FILE "build/ToolChain.cmake")
#SET(CMAKE_SYSTEM_NAME "Linux")
#SET(CMAKE_C_COMPILER "gcc")
#SET(CMAKE_CXX_COMPILER "g++")


project (HttpFlvServer)
SET(USE_STATIC_LIB_LINKAGE ON)

#SET(CMAKE_C_FLAGS "-g")
SET(CMAKE_CXX_FLAGS "-g -std=c++11 -pthread ")
# 设置C++标准为C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(src_list
    main.cpp
    HttpFlvServerDemo.cpp
    HttpFlvServerIO.cpp
	)		
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/../build/linux/${CMAKE_ToolChain}/net/include
    ${PROJECT_SOURCE_DIR}/../build/linux/${CMAKE_ToolChain}/lib/include
	)
ADD_EXECUTABLE(HttpFlvServer ${src_list})
TARGET_LINK_LIBRARIES(HttpFlvServer ${PROJECT_SOURCE_DIR}/../build/linux/${CMAKE_ToolChain}/net/libnet.a)
TARGET_LINK_LIBRARIES(HttpFlvServer ${PROJECT_SOURCE_DIR}/../build/linux/${CMAKE_ToolChain}/lib/libHttpFlvServer.a)
TARGET_LINK_LIBRARIES(HttpFlvServer ${PROJECT_SOURCE_DIR}/../build/linux/${CMAKE_ToolChain}/ThirdLib/http/libhttp.a)
TARGET_LINK_LIBRARIES(HttpFlvServer ${PROJECT_SOURCE_DIR}/../build/linux/${CMAKE_ToolChain}/ThirdLib/media/lib/libmedia.a)



SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)


#TARGET_LINK_LIBRARIES(crnode log4cpp thrift)










