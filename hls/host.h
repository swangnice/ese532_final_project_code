#ifndef HOST_H
#define HOST_H

#include "../Sever/encoder.h"
#include "../Sever/chunk.h"
#include "../Sever/cdc.h"
#include "../Sever/sha.h"
#include "../Sever/lzw.h"
//#include "lzw_encode.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "../Sever/server.h"
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "../Server/encoder.h"
#include "EventTimer.h"
#include "util.h"


#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

//this value should be more carefully defined afterwards
#define MAX_FILENAME_SIZE 100
#define NUM_KERNELS 4

#endif // HOST_H