#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS


#include "EventTimer.h"
#include <CL/cl2.hpp>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <vector>

#include "../Server/server.h"
#include "../Server/chunk.h"
#include "../Server/cdc.h"
#include "../Server/sha.h"
#include "../Server/lzw.h"
//#include "host.h"
#include "utils.h"

// max number of elements we can get from ethernet
#define NUM_ELEMENTS 16384
#define HEADER 2

#define NUM_PACKETS 8
#define pipe_depth 4
#define DONE_BIT_L (1 << 7)
#define DONE_BIT_H (1 << 15)
#define CHUNKS_IN_SINGLE_KERNEL 60
#define MAX_CHUNK_SIZE 4096
#define MAX_FILE_BUFFER_SIZE 4096

using namespace std;
int offset = 0;
unsigned char* file;

void handle_input(int argc, char* argv[], int* blocksize) {
	int x;
	extern char *optarg;

	while ((x = getopt(argc, argv, ":b:")) != -1) {
		switch (x) {
		case 'b':
			*blocksize = atoi(optarg);
			printf("blocksize is set to %d optarg\n", *blocksize);
			break;
		case ':':
			printf("-%c without parameter\n", optopt);
			break;
		}
	}
}

int main(int argc, char** argv)
{
    EventTimer timer;
// ------------------------------------------------------------------------------------
// Step 1: Initialize the OpenCL environment 
// ------------------------------------------------------------------------------------ 
    timer.add("OpenCL Initialization");
    cl_int err;
    std::string binaryFile = argv[1];
    unsigned fileBufSize;    
    std::vector<cl::Device> devices = get_xilinx_devices();
    devices.resize(1);
    cl::Device device = devices[0];
    cl::Context context(device, NULL, NULL, NULL, &err);
    char* fileBuf = read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    cl::Program program(context, devices, bins, NULL, &err);
    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
    
    cl::Kernel lzw_kernel = cl::Kernel(program,"lzw_compress", &err);
    
// ------------------------------------------------------------------------------------
// Step 2: Create buffers and initialize test values
// ------------------------------------------------------------------------------------

    unsigned char* lzw_s1;
    int *lzw_length;
    uint16_t* lzw_out_code;
    int *lzw_out_len;

    cl::Buffer lzw_s1_buf = cl::Buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY,  sizeof(unsigned char) * CHUNKS_IN_SINGLE_KERNEL * MAX_CHUNK_SIZE, NULL, &err);
    cl::Buffer lzw_length_buf = cl::Buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY, sizeof(int) * CHUNKS_IN_SINGLE_KERNEL, NULL, &err);
    cl::Buffer lzw_out_code_buf = cl::Buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY,  sizeof(uint16_t) * CHUNKS_IN_SINGLE_KERNEL * MAX_FILE_BUFFER_SIZE, NULL, &err);
    cl::Buffer lzw_out_len_buf = cl::Buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY,  sizeof(int) * CHUNKS_IN_SINGLE_KERNEL, NULL, &err);
	
    lzw_s1 = (unsigned char *)q.enqueueMapBuffer(lzw_s1_buf, CL_TRUE, CL_MAP_WRITE, 0, sizeof(unsigned char) * CHUNKS_IN_SINGLE_KERNEL * MAX_CHUNK_SIZE);
    lzw_length = (int*)q.enqueueMapBuffer(lzw_length_buf, CL_TRUE, CL_MAP_WRITE, 0, sizeof(int) * CHUNKS_IN_SINGLE_KERNEL);
    lzw_out_code = (uint16_t*)q.enqueueMapBuffer(lzw_out_code_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(uint16_t) * CHUNKS_IN_SINGLE_KERNEL * MAX_FILE_BUFFER_SIZE);
    lzw_out_len = (int*)q.enqueueMapBuffer(lzw_out_len_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(int) * CHUNKS_IN_SINGLE_KERNEL);
	
// ------------------------------------------------------------------------------------
// Step 3: Run the kernel
// ------------------------------------------------------------------------------------










































}
