cmake_minimum_required(VERSION 3.10) 


project (HttpFlvClientDemo)
 

#SET(CMAKE_C_FLAGS "-g")
SET(CMAKE_CXX_FLAGS "-g -std=c++11 -pthread ")
# 设置C++标准为C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message("CMAKE_SOURCE_DIR is: ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_LIST_DIR}")
  
  
# 查找线程库(跨平台)  会查找在当前平台上可用的线程库，并定义 Threads::Threads 目标。如果没有找到库，REQUIRED 会导致配置失败
if (UNIX AND NOT APPLE)  
    find_package(Threads REQUIRED) 
endif() 


# 收集所有源文件  
file(GLOB src_list "./*.cpp")  # 可以根据你的目录结构调整 

#set(src_list
#    src/xxx.cpp
#)

# 定义库  
add_executable(HttpFlvClientDemo ${src_list}) 

target_include_directories(HttpFlvClientDemo PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/net/include
    ${PROJECT_SOURCE_DIR}/../src/include
)

target_link_libraries(HttpFlvClientDemo PUBLIC net HttpFlvClient http media
#    ${CMAKE_BINARY_DIR}/demo/net/libnet.a
#    ${CMAKE_BINARY_DIR}/src/libHttpFlvClient.a
#    ${CMAKE_BINARY_DIR}/src/ThirdSrc/http/libhttp.a
#    ${CMAKE_BINARY_DIR}/src/ThirdSrc/media/libmedia.a
) 


if(WIN32)  
    # Windows平台上链接WinSock库  
    target_link_libraries(HttpFlvClientDemo PUBLIC ws2_32)
endif()


#target_link_libraries(HttpFlvClientDemo PRIVATE Threads::Threads)  
#TARGET_LINK_LIBRARIES(webrtcServer -pthread -lm -lrt -ldl -lresolv -static)

# 安装库文件  
install(TARGETS HttpFlvClientDemo  
        DESTINATION HttpFlvClientDemo) #安装目录 目录HttpFlvClientDemo换成目录. 也可以，这样就在当前目录生成不会生成在HttpFlvClientDemo目录下





#SET_TARGET_PROPERTIES(base64_static PROPERTIES OUTPUT_NAME base64)
#SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/lib)

#TARGET_LINK_LIBRARIES(webrtc pthread -lm)

#TARGET_LINK_LIBRARIES(crnode log4cpp thrift)










