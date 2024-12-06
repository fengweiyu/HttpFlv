cmake_minimum_required(VERSION 3.10) 


project (HttpFlvClient)
 
#子目录也会设置CMAKE_INSTALL_PREFIX，但是子目录退出后不生效，所以这里设置全局的，
#这样这里生成的Makefile可以执行install到下面这个目录，缺点是子目录相关的库文件头文件也会拷贝过来
set(CMAKE_INSTALL_PREFIX
    ${PROJECT_SOURCE_DIR}/build/linux/x64
)

# 添加 lib 子目录  
add_subdirectory(src/ThirdSrc/http)  
add_subdirectory(src/ThirdSrc/media) 

# 添加 src 子目录  
add_subdirectory(src)  


# 添加 demo 子目录  
add_subdirectory(demo/net) 
add_subdirectory(demo) 
