cmake_minimum_required(VERSION 2.8)             #确定版本号，必须步骤
project(HELLOENCLAVE)                           #项目名称，随便写
SET(SGX_SDK /home/cold/intel/sgxsdk)                  #SDK的安装路径
SET(SGX_MODE SIM)                                #参数的设置，注意，如果给变量赋值的时候，值本身有空格，那么值需要使用“”来保证赋值的正确性，否则赋值可能存在错误
SET(SGX_ARCH x64)
SET(SGX_DEBUG 1)

IF(${SGX_MODE} EQUAL HW)
  IF(${SGX_DEBUG} EQUAL 1)
    SET(Build_Mode HW_DEBUG)
  ELSEIF(${SGX_PRERELEASE} EQUAL 1)
    SET(Build_Mode HW_PRERELEASE)
  ELSE()
	  SET(Build_Mode HW_RELEASE)
  ENDIF()
ELSE()
  IF(${SGX_DEBUG} EQUAL 1)
  SET(Build_Mode SIM_DEBUG)
  ELSEIF(${SGX_PRERELEASE} EQUAL 1)
  SET(Build_Mode SIM_PRERELEASE)
  ELSE()
  SET(Build_Mode SIM_RELEASE)
ENDIF()
ENDIF()

SET(SGX_COMMON_CFLAGS -m64)
SET(SGX_LIBRARY_PATH ${SGX_SDK}/lib64)
SET(SGX_EDGER8R ${SGX_SDK}/bin/x64/sgx_edger8r)
SET(SGX_ENCLAVE_SIGNER ${SGX_SDK}/bin/x64/sgx_sign) #以上的参数都是根据自己的机器设置，注意不要设置错误
IF(SGX_DEBUG EQUAL 1)
   SET(SGX_COMMON_CFLAGS "${SGX_COMMON_CFLAGS} -O0 -g")
ELSE(SGX_DEBUG EQUAL 1)
   SET(SGX_COMMON_CFLAGS "${SGX_COMMON_CFLAGS} -O2")
ENDIF(SGX_DEBUG EQUAL 1)

add_subdirectory(App)                           #执行App文件夹中的CMakeLists.txt
add_subdirectory(Enclave)                       #执行Enclave文件夹中的CMakeLists.txt
