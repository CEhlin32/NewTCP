INC_PATHS = $(INC) -Iinclude -I../Utilities/include -I../Devices/include  -I../AESCrypto/include
LIB_NAME = NewTCP.so

$(info  ************************************)
$(info  ********** Building NewTCP $(CMD) )
$(info  ************************************)

MAKE_TYPE = LIB
export MAKE_TYPE
export LIB_NAME
export INC_PATHS

include ../../MakeSupport/MakefileWithRev.mak 
