mkdir build
cd build
/home/cold/intel/sgxsdk/bin/x64/sgx_edger8r --untrusted ../Enclave/Enclave.edl   --search-path ../Enclave --search-path /home/cold/intel/sgxsdk/include/
/home/cold/intel/sgxsdk/bin/x64/sgx_edger8r --trusted ../Enclave/Enclave.edl --search-path ../Enclave --search-path /home/cold/intel/sgxsdk/include/

cmake ..
make 
/home/cold/intel/sgxsdk/bin/x64/sgx_sign sign -key ../Enclave/Enclave_private.pem -enclave Enclave/libenclave.so -out App/enclave.signed.so -config ../Enclave/Enclave.config.xml
