IF(${SGX_MODE} EQUAL HW)
SET(Urts_Library_Name sgx_urts)
SET(Uae_Service_Library_Name sgx_uae_service)
ELSE()
SET(Urts_Library_Name sgx_urts_sim)
SET(Uae_Service_Library_Name sgx_uae_service_sim)
ENDIF()

FILE(GLOB_RECURSE App_C_Files "*.c")
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/Include ${PROJECT_SOURCE_DIR}/App ${SGX_SDK}/include)  #包含所需要的头文件，后面记得链接,相当于Makefile文件中“-L”选项
SET(Mysql "mysqlclient")
SET(App_C_Flags "${SGX_COMMON_CFLAGS} -lmysqlclient -fPIC -Wno-attributescmake")#设置编译App文件夹中C文件的文件选项
SET(App_C_Flags "${App_C_Flags} -DDEBUG -UNDEBUG -UEDEBUG")
SET(App_Cpp_Flags "${App_C_Flags}")
SET(App_Link_Flags "${SGX_COMMON_CFLAGS} -L${SGX_SDK}/lib64 -l${Urts_Library_Name} -lpthread -l${Uae_Service_Library_Name}")    #设置生成应用程序的链接选项，因为要>用到SDK中带有的库
SET(CMAKE_CXX_FLAGS ${App_Cpp_Flags})         #将CMake中编译CPP文件中的编译选项设置成我们自己的选项
SET(CMAKE_C_FLAGS ${App_C_Flags})
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${App_Link_Flags}")#设置链接生成可执行文件的时候的链接选项
SET(CMAKE_CXX_STANDARD 11)   #设置C++的std，相当于makefile文件中的-std=c++11
LINK_DIRECTORIES("/usr/lib")    #注意，不可信部分链接的库不再${SDK}/lib64中，而是在/usr/lib中，通过本条指令，这样在链接的时候，我们使用的库Cmake就会在/usr/lib中寻找
ADD_EXECUTABLE(main ${App_C_Files} ../build/Enclave_u.c)#生成可执行文件，这里的可执行文件我们定义的名称是main
TARGET_LINK_LIBRARIES(main ${Urts_Library_Name}  mysqlclient z m rt atomic ssl crypto dl pthread pbc gmp ${Uae_Service_Library_Name}) #可执行文件需要的库，注意链接库的先后顺序，有的库需要依赖前面的库函数，如果顺序错误，可能>会报错，说找不到相应函数

