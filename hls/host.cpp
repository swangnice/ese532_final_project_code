#include "host.h"

using namespace std;
int offset = 0;
unsigned char* file;

static void write_encoded_file_buf(uint16_t* out_code, int out_len, unsigned char* file_buffer, int &total_bytes){
    int total_bits = out_len * 12;
    total_bytes = static_cast<int>(std::ceil(total_bits / 8.0));
    uint32_t header = static_cast<uint32_t>(total_bytes & 0xFFFFFFFF) << 1;

    int i = 0, j = 0;

    file_buffer[j++] = static_cast<unsigned char>(header & 0xFF);
    file_buffer[j++] = static_cast<unsigned char>((header >> 8) & 0xFF);
    file_buffer[j++] = static_cast<unsigned char>((header >> 16) & 0xFF);
    file_buffer[j++] = static_cast<unsigned char>(header >> 24);
    for(i = 0; i + 1 < out_len; i += 2){
        file_buffer[j++] = static_cast<unsigned char>(out_code[i] >> 4);
        file_buffer[j++] = static_cast<unsigned char>(((out_code[i] << 4) & 0xF0) | ((out_code[i + 1] >> 8) & 0x0F));
        file_buffer[j++] = static_cast<unsigned char>(out_code[i + 1] & 0xFF);
    }
    if(i != out_len){
        file_buffer[j++] = static_cast<unsigned char>(out_code[i] >> 4);
        file_buffer[j++] = static_cast<unsigned char>((out_code[i] << 4) & 0xF0);
    }
    return;
}

static void vector_to_array(vector<uint16_t> vec, uint16_t* arr, int &len){
    len = vec.size();
    for(int i = 0; i < len; i++){
        arr[i] = vec[i];
    }
    return;
}

static void write_file(unsigned char* file_buffer, int total_bytes, char* fileName){
    std::ofstream outfile(fileName, std::ios::app);
    
    if(!outfile.is_open()) {
        std::cerr << "Could not open the file for writing.\n";
        return;
    }

    // Write the data to the file
    outfile.write(reinterpret_cast<const char*>(file_buffer), total_bytes + 4);

    // Check for write errors
    if (!outfile.good()) {
        std::cerr << "Error occurred while writing to the file.\n";
    }

    // Close the file
    outfile.close();
}



int main(int argc, char** argv)
{
// Initialize an event timer we'll use for monitoring the application
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

	//implement multi-kernels
	cl::Kernel lzw_kernel;

	lzw_kernel = cl::Kernel(program,"lzw_compression", &err);

// ------------------------------------------------------------------------------------
// Step 2: Create buffers and initialize test values
// ------------------------------------------------------------------------------------
    timer.add("Allocate contiguous OpenCL buffers");
    // Create the buffers and allocate memory   
    cl::Buffer lzw_chunks_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY,  sizeof(unsigned char) * CHUNKS_IN_SINGLE_KERNEL * MAX_CHUNK_SIZE, NULL, &err);
	cl::Buffer lzw_chunks_length_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY, sizeof(int) * CHUNKS_IN_SINGLE_KERNEL, NULL, &err);
    cl::Buffer lzw_file_buffer_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY,  sizeof(unsigned char) * CHUNKS_IN_SINGLE_KERNEL * MAX_FILE_BUFFER_SIZE, NULL, &err);
    cl::Buffer lzw_total_bytes_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY,  sizeof(int) * CHUNKS_IN_SINGLE_KERNEL, NULL, &err);

	unsigned char* lzw_chunks;
	int *lzw_chunks_length;
    unsigned char* lzw_file_buffer;
	int *lzw_total_bytes;

	lzw_chunks = (unsigned char *)q.enqueueMapBuffer(lzw_chunks_buf, CL_TRUE, CL_MAP_WRITE, 0, sizeof(unsigned char) * CHUNKS_IN_SINGLE_KERNEL * MAX_CHUNK_SIZE);
	lzw_chunks_length = (int*)q.enqueueMapBuffer(lzw_chunks_length_buf, CL_TRUE, CL_MAP_WRITE, 0, sizeof(int) * CHUNKS_IN_SINGLE_KERNEL);
	lzw_file_buffer = (unsigned char*)q.enqueueMapBuffer(lzw_file_buffer_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(unsigned char) * CHUNKS_IN_SINGLE_KERNEL * MAX_FILE_BUFFER_SIZE);
	lzw_total_bytes = (int*)q.enqueueMapBuffer(lzw_total_bytes_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(int) * CHUNKS_IN_SINGLE_KERNEL);

    timer.add("Populating buffer inputs");

// ------------------------------------------------------------------------------------
// Step 3: Run the kernel
// ------------------------------------------------------------------------------------
	std::cout << "dec 9 0105" << std::endl;
	std::cout << argv[1] << std::endl;
	stopwatch ethernet_timer;
	stopwatch cdc_timer;
	stopwatch sha_timer;
	stopwatch dedup_timer;
	stopwatch lzw_timer;
	stopwatch overall_timer;
	unsigned char* input[NUM_PACKETS];
	int writer = 0;
	int done = 0;
	int length = 0;
	int count = 0;
	ESE532_Server server;

	// std::vector<cl::Event> read_events[NUM_KERNELS];

	// default is 2k
	int blocksize = BLOCKSIZE;

	// set blocksize if decalred through command line
	handle_input(argc, argv, &blocksize);

	file = (unsigned char*) malloc(sizeof(unsigned char) * 70000000);
	if (file == NULL) {
		printf("help\n");
	}

	for (int i = 0; i < NUM_PACKETS; i++) {
		input[i] = (unsigned char*) malloc(
				sizeof(unsigned char) * (NUM_ELEMENTS + HEADER));
		if (input[i] == NULL) {
			std::cout << "aborting " << std::endl;
			return 1;
		}
	}

	server.setup_server(blocksize);

	writer = pipe_depth;
	server.get_packet(input[writer]);
	count++;

    // get packet
	unsigned char* buffer = input[writer];

	// decode
	done = buffer[1] & DONE_BIT_L;
	length = buffer[0] | (buffer[1] << 8);
	length &= ~DONE_BIT_H;
	// printing takes time so be weary of transfer rate
	//printf("length: %d offset %d\n",length,offset);

	// we are just memcpy'ing here, but you should call your
	// top function here.
	overall_timer.start();
	memcpy(&file[offset], &buffer[HEADER], length);
	//printf("%.*s\n", length, &buffer[HEADER]);
	unsigned char **chunks = NULL;
    unsigned int chunk_count = 0;
    unsigned int *chunk_sizes = NULL;
	chunk_count = 0;
    unsigned int estimated_chunks = 15360 / WIN_SIZE + 1;
    chunks = (unsigned char **)malloc(sizeof(unsigned char *) * estimated_chunks);
    chunk_sizes = (unsigned int *)malloc(sizeof(unsigned int) * estimated_chunks);
	cdc_timer.start();
	cdc(&buffer[HEADER], length, &chunks, &chunk_count, &chunk_sizes);




	offset += length;
	writer++;

    //last message
	while (!done) {
		// reset ring buffer
		if (writer == NUM_PACKETS) {
			writer = 0;
		}

		ethernet_timer.start();
		server.get_packet(input[writer]);
		ethernet_timer.stop();

		count++;

		// get packet
		unsigned char* buffer = input[writer];

		// decode
		done = buffer[1] & DONE_BIT_L;
		length = buffer[0] | (buffer[1] << 8);
		length &= ~DONE_BIT_H;
		//printf("length: %d offset %d\n",length,offset);
		memcpy(&file[offset], &buffer[HEADER], length);
		
		cdc(&buffer[HEADER], length, &chunks, &chunk_count, &chunk_sizes);
		

		offset += length;
		writer++;
	}

    cdc_timer.stop();

	FILE *outfd = fopen("output_cpu.bin", "wb");
	int bytes_written = fwrite(&file[0], 1, offset, outfd);
	printf("write file with %d\n", bytes_written);
	fclose(outfd);

    sha_timer.start();
	uint8_t sha256_output[chunk_count][32];

	for (unsigned int i = 0; i < chunk_count; i++) {
		unsigned char *temp_chunk_data = chunks[i];
    	unsigned int temp_chunk_size = chunk_sizes[i];

		unsigned char compressed_data[SHA_BLOCKSIZE];
    	int compressed_size = rle_compress((const unsigned char*)temp_chunk_data, temp_chunk_size, compressed_data, SHA_BLOCKSIZE);

		//std::cout << "Original Size: " << temp_chunk_size << " bytes\n";
		//std::cout << "Compressed Size: " << compressed_size << " bytes\n";

		// 计算压缩数据的 SHA-256 哈希
		uint8_t temp_sha256_output[32]; // SHA-256 输出为 32 字节的 uint8_t 数组
		calculate_sha256(compressed_data, compressed_size, temp_sha256_output);

		// 输出 SHA-256 哈希结果
		std::cout << "SHA-256 Hash of Compressed Data: ";
		for (int i = 0; i < 32; i++) {
			printf("%02x", temp_sha256_output[i]);  // 打印每个字节
		}
		std::cout << std::endl;


		memcpy(sha256_output[i], temp_sha256_output, 32);
		
	}

	sha_timer.stop();
	//int dict[MAX_DICT_SIZE][256];

	//init_dict(dict);
	
	//deduplication and assign lzw header
	dedup_timer.start();
	uint8_t dup_flag[chunk_count];
	int dup_index[chunk_count];
	dup_flag[0] = 0;	
	dup_index[0] = 0;
	int undup_count = 1;
	for (unsigned int i = 1; i < chunk_count; i++) {				
		dup_flag[i] = 0;	//un duplicated
		dup_index[i] = i;
		for (unsigned int j = 0; j < i; j++) {
			if (memcmp(sha256_output[i], sha256_output[j], 32) == 0) {
				dup_flag[i] = 1;
				dup_index[i] = j;
				break;
			}
		}
		undup_count++;
	}
	for (unsigned int i = 0; i < chunk_count; i++) {
		printf("Chunk %u: %d, duplicated with %d\n", i, dup_flag[i], dup_index[i]);
	}
	dedup_timer.stop();

    	//gunsigned char* buffer_out = (unsigned char*) malloc(sizeof(unsigned char) * 70000000);
	//compress unduplicated chunks
	lzw_timer.start();
	uint8_t lzw_compressed_output[undup_count][2048];
	
	int compressed_data_size[undup_count];
	for (unsigned int i = 0; i < chunk_count; i++) {
		if (dup_flag[i] == 0) {
            //memcpy data
            memcpy(&lzw_chunks, chunks[i], chunk_sizes[i]);
            lzw_chunks_length = chunk_sizes[i];

            //set arg
            lzw_kernel.setArg(0, lzw_chunks_buf);
            lzw_kernel.setArg(1, lzw_chunks_length_buf);
            lzw_kernel.setArg(2, lzw_file_buffer_buf);
            lzw_kernel.setArg(3, lzw_total_bytes_buf);

            std::vector<cl::Event> write_events;
			std::vector<cl::Event> exec_events;

            cl::Event write_ev;
			cl::Event exec_ev;
			cl::Event read_ev;

			q.enqueueMigrateMemObjects({lzw_chunks_buf, lzw_chunks_length_buf}, 0 /* 0 means from host*/, NULL, &write_ev);
			write_events.push_back(write_ev); 	

			printf("enqueue task\n");
			q.enqueueTask(lzw_kernel, &write_events, &exec_ev);
			exec_events.push_back(exec_ev);

			printf("Memory object migration enqueue host->device\n");
			q.enqueueMigrateMemObjects({lzw_file_buffer_buf, lzw_total_bytes_buf}, CL_MIGRATE_MEM_OBJECT_HOST, &exec_events, &read_ev);
			q.finish();

			int output_index = convert_output(lzw_file_buf, lzw_compressed_output[i], lzw_total_bytes_buf);
			compressed_data_size[i] = output_index;

			// int temp_output_index = 0;
			
			// uint16_t temp_lzw_compressed_output[chunk_sizes[i]];


			// //std::string temp_chunk = reinterpret_cast<const char*>(chunks[i]);
			// //const unsigned char* temp_data = reinterpret_cast<const unsigned char*>(temp_chunk.c_str());
			// lzw_compress(chunks[i], chunk_sizes[i], temp_lzw_compressed_output, &temp_output_index);


			// //lzw(chunks[i], chunk_sizes[i], dict, temp_lzw_compressed_output, temp_output_index);
			// int output_index = convert_output(temp_lzw_compressed_output, lzw_compressed_output[i], temp_output_index);
			// compressed_data_size[i] = output_index;
		}
	}

	// //last message
	// while (!done) {
	// 	ticks++;
	// 	// reset ring buffer
	// 	if (writer == NUM_PACKETS) {
	// 		writer = 0;
	// 	}

	// 	ethernet_timer.start();
	// 	server.get_packet(input[writer]);
	// 	ethernet_timer.stop();

	// 	count++;

	// 	// get packet
	// 	unsigned char* buffer = input[writer];

	// 	// decode
	// 	done = buffer[1] & DONE_BIT_L;
	// 	length = buffer[0] | (buffer[1] << 8);
	// 	length &= ~DONE_BIT_H;

	// 	total_inputBits+=length;
	// 	//printf("length: %d offset %d\n",length,offset);
	// 	memcpy(&file[offset], &buffer[HEADER], length);

	// 	offset += length;
	// 	writer++;

	// 	// encode the obtained information in buffer

	// 	// initialize the vector to store the obtained chunks
	// 	unsigned char** chunks = NULL;
    //     unsigned int chunk_count = 0;
    //     unsigned int* chunk_sizes NULL;

    //     chunk_count = 0;

    //     unsigned int estimated_chunks = 15360 / WIN_SIZE + 1;
    //     chunks = (unsigned char **)malloc(sizeof(unsigned char*) * estimated_chunks);
    //     chunk_sizes = (unsigned int *)malloc(sizeof(unsigned int) * estimated_chunks);

	// 	total_time.start();
	// 	cdc_time.start();
	// 	cdc(&buffer[HEADER], length, &chunks, &chunk_count, &chunk_sizes);
	// 	cdc_time.stop();

	// 	std::vector<bool> lzw_or_dedup; //0 denotes lzw, while 1 denotes dedup

	// 	int lzw_offset = 0;

	// 	std::vector<std::array<unsigned char, 4>> dedup_file_buffer;
	// 	int dedup_offset = 0;

	// 	//calculate hash value and chunk id for each chunk
	// 	//add those key-value pairs to chunks map
	// 	//Question: do we need to consider the situation that different chunks share the same hash value calculated by SHA at this point
	// 	for(std::vector<std::string>::size_type i = 0; i < chunks.size(); i++){
	// 	// for(std::vector<std::string>::size_type i = 0; i < 10; i++){
	// 		hash_part hash_value;
	// 		sha_time.start();
	// 		sha(chunks[i], hash_value);
	// 		sha_time.stop();

	// 		std::string hash_hex_string = toHexString(hash_value);
			
	// 		if(chunks_map.find(hash_hex_string) == chunks_map.end()){
	// 			chunks_map.insert({hash_hex_string, base + i});
	// 			unsigned char* chunk_content = (unsigned char*)malloc(sizeof(unsigned char) * MAX_CHUNK_SIZE);
    //             convert_string_char(chunks[i], chunk_content);
    //             int chunk_len = chunks[i].length();
	// 			int total_bytes = 0;
				
	// 			//running kernel on FPGA
	// 			memcpy(&lzw_chunks[lzw_offset % NUM_KERNELS][(lzw_offset / NUM_KERNELS) * MAX_CHUNK_SIZE], chunk_content, chunks[i].length());
	// 			lzw_chunks_length[lzw_offset % NUM_KERNELS][(lzw_offset / NUM_KERNELS)] = chunk_len;

	// 			free(chunk_content);

	// 			lzw_or_dedup.push_back(false);
	// 			lzw_offset++;

	// 			if(lzw_offset == NUM_KERNELS * CHUNKS_IN_SINGLE_KERNEL){
	// 				std::cout << "calculating lzw in multi chunks" << std::endl;
	// 				lzw_offset = 0;
 	// 				// running kernel on FPGA
	// 				printf("Set kernel arguments\n");  
	// 				// Map buffers to kernel arguments, thereby assigning them to specific device memory banks

	// 				for(int k = 0; k < NUM_KERNELS; k++){
	// 					multi_kernels[k].setArg(0, lzw_chunks_bufs[k]);
	// 					multi_kernels[k].setArg(1, lzw_chunks_length_bufs[k]);
	// 					multi_kernels[k].setArg(2, lzw_file_buffer_bufs[k]);
	// 					multi_kernels[k].setArg(3, lzw_total_bytes_bufs[k]);
	// 				}

	// 				printf("Memory object migration enqueue host->device\n");

	// 				std::vector<cl::Event> write_events[NUM_KERNELS];
	// 				std::vector<cl::Event> exec_events[NUM_KERNELS];

	// 				cl::Event write_ev[NUM_KERNELS];
	// 				cl::Event exec_ev[NUM_KERNELS];
	// 				cl::Event read_ev[NUM_KERNELS];

	// 				for(int k = 0; k < NUM_KERNELS; k++){
	// 					q.enqueueMigrateMemObjects({lzw_chunks_bufs[k], lzw_chunks_length_bufs[k]}, 0 /* 0 means from host*/, NULL, &write_ev[k]);
	// 					write_events[k].push_back(write_ev[k]); 	
	// 				}

	// 				printf("enqueue task\n");
	// 				for(int k = 0; k < NUM_KERNELS; k++){
	// 					q.enqueueTask(multi_kernels[k], &write_events[k], &exec_ev[k]);
	// 					exec_events[k].push_back(exec_ev[k]);
	// 				}

	// 				printf("Memory object migration enqueue host->device\n");
	// 				for(int k = 0; k < NUM_KERNELS; k++){
	// 					q.enqueueMigrateMemObjects({lzw_file_buffer_bufs[k], lzw_total_bytes_bufs[k]}, CL_MIGRATE_MEM_OBJECT_HOST, &exec_events[k], &read_ev[k]);
	// 				} 
	// 				q.finish();

	// 				for(int i = 0; i < lzw_or_dedup.size(); i++){
	// 					if(!lzw_or_dedup[i]){
	// 						write_file(&lzw_file_buffer[lzw_offset % NUM_KERNELS][(lzw_offset / NUM_KERNELS) * MAX_FILE_BUFFER_SIZE], lzw_total_bytes[lzw_offset % NUM_KERNELS][(lzw_offset / NUM_KERNELS)], "encoded.bin");
	// 						lzw_offset++;
	// 					}
	// 					else{
	// 						unsigned char write_temp[4];
	// 						std::memcpy(write_temp, dedup_file_buffer[dedup_offset].data(), 4);
	// 						write_file(write_temp, 0, "encoded.bin");
	// 						dedup_offset++;
	// 					}
	// 				}
	// 				lzw_offset = 0;
	// 				dedup_offset = 0;
	// 				lzw_or_dedup.clear();
	// 				dedup_file_buffer.clear();
	// 			}
	// 			total_time.stop();
	// 			sum_lzw_raw_length += chunks[i].length() * 1.5;
	// 			sum_lzw_cmprs_len += total_bytes;
	// 			total_time.start();
	// 		}

	// 		else{
	// 			uint32_t out_code;
	// 			dedup_chars += chunks[i].length();
	// 			dedup_time.start();
	// 			unsigned char* dedup_file_buffer_temp = (unsigned char*)malloc(sizeof(unsigned char) * 4);
	// 			std::array<unsigned char, 4> dedup_arr_temp;
	// 			duplicate_encoding(chunks_map.at(hash_hex_string), out_code, dedup_file_buffer_temp);
	// 			std::memcpy(dedup_arr_temp.data(), dedup_file_buffer_temp, 4);
	// 			free(dedup_file_buffer_temp);
	// 			dedup_file_buffer.push_back(dedup_arr_temp);
	// 			lzw_or_dedup.push_back(true);
	// 			dedup_time.stop();
	// 			total_time.stop();
	// 			sum_dedup_raw_length += chunks[i].length() * 1.5;
	// 			sum_dedup_cmprs_len += 4;
	// 			total_time.start();
	// 		}
	// 	}
	// 	if(lzw_offset != 0){
	// 		std::cout << "calculating lzw in multi chunks" << std::endl;
	// 		lzw_offset = 0;

	// 		// running kernel on FPGA
	// 		timer.add("Set kernel arguments");  
	// 		// Map buffers to kernel arguments, thereby assigning them to specific device memory banks

	// 		for(int k = 0; k < NUM_KERNELS; k++){
	// 			multi_kernels[k].setArg(0, lzw_chunks_bufs[k]);
	// 			multi_kernels[k].setArg(1, lzw_chunks_length_bufs[k]);
	// 			multi_kernels[k].setArg(2, lzw_file_buffer_bufs[k]);
	// 			multi_kernels[k].setArg(3, lzw_total_bytes_bufs[k]);
	// 		}

	// 		timer.add("Memory object migration enqueue host->device");

	// 		std::vector<cl::Event> write_events[NUM_KERNELS];
	// 		std::vector<cl::Event> exec_events[NUM_KERNELS];

	// 		cl::Event write_ev[NUM_KERNELS];
	// 		cl::Event exec_ev[NUM_KERNELS];
	// 		cl::Event read_ev[NUM_KERNELS];

	// 		for(int k = 0; k < NUM_KERNELS; k++){
	// 			q.enqueueMigrateMemObjects({lzw_chunks_bufs[k], lzw_chunks_length_bufs[k]}, 0 /* 0 means from host*/, NULL, &write_ev[k]);
	// 			write_events[k].push_back(write_ev[k]); 	
	// 		}

	// 		for(int k = 0; k < NUM_KERNELS; k++){
	// 			q.enqueueTask(multi_kernels[k], &write_events[k], &exec_ev[k]);
	// 			exec_events[k].push_back(exec_ev[k]);
	// 		}

	// 		for(int k = 0; k < NUM_KERNELS; k++){
	// 			q.enqueueMigrateMemObjects({lzw_file_buffer_bufs[k], lzw_total_bytes_bufs[k]}, CL_MIGRATE_MEM_OBJECT_HOST, &exec_events[k], &read_ev[k]);
	// 		} 
	// 		q.finish();

	// 		for(int i = 0; i < lzw_or_dedup.size(); i++){
	// 			if(!lzw_or_dedup[i]){
	// 				write_file(&lzw_file_buffer[lzw_offset % NUM_KERNELS][(lzw_offset / NUM_KERNELS) * MAX_FILE_BUFFER_SIZE], lzw_total_bytes[lzw_offset % NUM_KERNELS][(lzw_offset / NUM_KERNELS)], "encoded.bin");
	// 				lzw_offset++;
	// 			}
	// 			else{
	// 				unsigned char write_temp[4];
	// 				std::memcpy(write_temp, dedup_file_buffer[dedup_offset].data(), 4);
	// 				write_file(write_temp, 0, "encoded.bin");
	// 				dedup_offset++;
	// 			}
	// 		}
	// 		lzw_offset = 0;
	// 		dedup_offset = 0;
	// 		lzw_or_dedup.clear();
	// 		dedup_file_buffer.clear();
	// 	}
	// 	base += chunks.size();
	// }
    lzw_timer.stop();

    	// build header
	uint32_t header[chunk_count];
	for (unsigned int i = 0; i < chunk_count; i++) {

		header[i] = 0;
		if (dup_flag[i] == 0) {
			header[i] = compressed_data_size[i] << 1;
		} else {
			header[i] = (dup_index[i]<<1) | 0x00000001;
		}
		//header[i] = (temp_header << 24) & 0xff000000 | (temp_header<<8)&0x00ff0000 | (temp_header>>8)&0x0000ff00 | (temp_header>>24)&0x000000ff;

		//printf("chunk: %u, Header: %#010x\n", i, header[i]);
		//printf("chunk: %u, Header: %#010x\n", i, temp_header);
	}

	total_time.stop();




	//write in output bin
	FILE* out_file = fopen("compressed_data.bin", "wb");
	if (file == NULL) {
        perror("Failed to open file");
    }

    for (unsigned int i = 0; i < chunk_count; i++) {
		fwrite(&header[i], sizeof(uint32_t), 1, out_file);
		printf("%#010x", header[i]);
		if (dup_flag[i] == 0) {
			for (int j = 0; j < compressed_data_size[i]; j++) {
				fwrite(&lzw_compressed_output[i][j], sizeof(uint8_t), 1, out_file);
				printf("%02X ", lzw_compressed_output[i][j]);
			}
		}
	}

    fclose(out_file);

    for (int i = 0; i < NUM_PACKETS; i++) {
		free(input[i]);
	}

	for (unsigned int i = 0; i < chunk_count; ++i) {
        free(chunks[i]);
    }

    free(chunks);
    free(chunk_sizes);

	std::cout << "--------------- Key Throughputs ---------------" << std::endl;
	float ethernet_latency = ethernet_timer.latency() / 1000.0;
	float input_throughput = (bytes_written * 8 / 1000000.0) / ethernet_latency; // Mb/s
	std::cout << "Input Throughput to Encoder: " << input_throughput << " Mb/s."
			<< " (Latency: " << ethernet_latency << "s)." << std::endl;

	float cdc_latency = cdc_time.latency() / 1000.0;
	float cdc_throughput = (bytes_written * 8 / 1000000.0) / cdc_latency; // Mb/s
	std::cout << "cdc Throughput to Encoder: " << cdc_throughput << " Mb/s."
			<< " (Latency: " << cdc_latency << "s)." << std::endl;

	float sha_latency = sha_time.latency() / 1000.0;
	float sha_throughput = (bytes_written * 8 / 1000000.0) / sha_latency; // Mb/s
	std::cout << "sha Throughput to Encoder: " << sha_throughput << " Mb/s."
			<< " (Latency: " << sha_latency << "s)." << std::endl;

	float dedup_latency = dedup_time.latency() / 1000.0;
	float dedup_throughput = (dedup_chars * 8 / 1000000.0) / dedup_latency; // Mb/s
	std::cout << "dedup Throughput to Encoder: " << dedup_throughput << " Mb/s."
			<< " (Latency: " << dedup_latency << "s)." << std::endl;

	float output_latency = total_time.latency() / 1000.0;
	float output_throughput = (total_inputBits * 8 / 1000000.0) / output_latency; // Mb/s
	std::cout << "output Throughput to Encoder: " << output_throughput << " Mb/s."
			<< " (Latency: " << output_latency << "s)." << std::endl;

// ------------------------------------------------------------------------------------
// Step 4: Check Results and Release Allocated Resources
// ------------------------------------------------------------------------------------
    // multiply_gold(in1, in2, out_sw);
    // bool match = Compare_matrices(out_sw, out_hw);
    // Destroy_matrix(out_sw);
    delete[] fileBuf;
	q.enqueueUnmapMemObject(lzw_chunks_buf, lzw_chunks);
	q.enqueueUnmapMemObject(lzw_chunks_length_buf, lzw_chunks_length);
	q.enqueueUnmapMemObject(lzw_file_buffer_buf, lzw_file_buffer);
	q.enqueueUnmapMemObject(lzw_total_bytes_buf, lzw_total_bytes);
    q.finish();

    // std::cout << "--------------- Key execution times ---------------" << std::endl;
    // timer.print();

    // std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl; 
    // return (match ? EXIT_SUCCESS : EXIT_FAILURE);
    return 0;
}