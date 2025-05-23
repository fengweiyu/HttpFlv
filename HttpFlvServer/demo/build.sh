#!/bin/bash

function PrintUsage()
{
    echo -e "Usage:"
    echo -e "./build.sh $ToolChain"
    echo -e "ToolChain: arm-linux/x86/x64"
    echo -e "EGG:"
    echo -e "./build.sh x86"
    echo -e " or ./build.sh x64"
}
function GenerateCmakeFile()
{
#   mkdir -p build
    CmakeFile="$2/ToolChain.cmake"
    echo "SET(CMAKE_SYSTEM_NAME \"Linux\")" > $CmakeFile
    if [ $1 == x86 -o $1 == x64 ]; then
        echo "SET(CMAKE_C_COMPILER \"gcc\")" >> $CmakeFile  
        echo "SET(CMAKE_CXX_COMPILER \"g++\")" >> $CmakeFile    
        echo "SET(CMAKE_ToolChain \"$1\")" >> $CmakeFile        
    else
        echo "SET(CMAKE_C_COMPILER \"$1-gcc\")" >> $CmakeFile
        echo "SET(CMAKE_CXX_COMPILER \"$1-g++\")" >> $CmakeFile
        echo "SET(CMAKE_ToolChain \"$1\")" >> $CmakeFile        
    fi
}
function BuildExe()
{
    echo -e "Start building HttpFlvServer..."
    OutputPath="./build"
    if [ -e "$OutputPath" ]; then
        rm $OutputPath -rf
#防止切换平台编译时由于平台不对应报错，所以删除build重新建立       
#       if [ -e "$OutputPath/lib" ]; then
#           echo "/build/lib exit"
#       else
#           mkdir $OutputPath/lib
#       fi
#   else
#       mkdir $OutputPath
#       mkdir $OutputPath/lib
    fi  
    mkdir $OutputPath
    
    GenerateCmakeFile $1 $OutputPath    
    cd $OutputPath
    cmake ..
    if [ -e "Makefile" ]; then  
        make clean
        make -j4 > /dev/null
        if [ $? == 0 ]; then
            echo "make success! "
        else
            echo "make failed! "
            exit -1
        fi
    else
        echo "Makefile generated failed! "
        exit -1
    fi
    cd ..
}

function CopyExe()
{
#   CurPwd = $PWD
    CurPwd=$PWD
    cd $1

    
    cp $CurPwd/build/HttpFlvServer .

#由于对外头文件又依赖内部头文件，所以要拷贝，暂时这么处理后续优化   
#    cp $CurPwd/*.h .
}

if [ $# == 0 ]; then
    PrintUsage
    exit -1
else
    cd net
    sh build.sh $1 $2
    if [ $? -ne 0 ]; then
        exit -1
    fi
    cd ..
    
    BuildExe $1
    CopyExe $2
fi




