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
	std::vector<cl::Kernel> multi_kernels(NUM_KERNELS);

	for(int i = 0; i < NUM_KERNELS; i++){
		multi_kernels[i] = cl::Kernel(program,"lzw_multi_chunks", &err);
	}

// ------------------------------------------------------------------------------------
// Step 2: Create buffers and initialize test values
// ------------------------------------------------------------------------------------
    timer.add("Allocate contiguous OpenCL buffers");
    // Create the buffers and allocate memory   
    // cl::Buffer lzw_chunks_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY,  sizeof(unsigned char) * CHUNKS_IN_SINGLE_KERNEL * MAX_CHUNK_SIZE, NULL, &err);
	// cl::Buffer lzw_chunks_length_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY, sizeof(int) * CHUNKS_IN_SINGLE_KERNEL, NULL, &err);
    // cl::Buffer lzw_file_buffer_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY,  sizeof(unsigned char) * CHUNKS_IN_SINGLE_KERNEL * MAX_FILE_BUFFER_SIZE, NULL, &err);
    // cl::Buffer lzw_total_bytes_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY,  sizeof(int) * CHUNKS_IN_SINGLE_KERNEL, NULL, &err);

	std::vector<cl::Buffer> lzw_chunks_bufs(NUM_KERNELS);
	std::vector<cl::Buffer> lzw_chunks_length_bufs(NUM_KERNELS);
	std::vector<cl::Buffer> lzw_file_buffer_bufs(NUM_KERNELS);
	std::vector<cl::Buffer> lzw_total_bytes_bufs(NUM_KERNELS);

	for(int i = 0; i < NUM_KERNELS; i++){
		lzw_chunks_bufs[i] = cl::Buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY,  sizeof(unsigned char) * CHUNKS_IN_SINGLE_KERNEL * MAX_CHUNK_SIZE, NULL, &err);
		lzw_chunks_length_bufs[i] = cl::Buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY, sizeof(int) * CHUNKS_IN_SINGLE_KERNEL, NULL, &err);
		lzw_file_buffer_bufs[i] = cl::Buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY,  sizeof(unsigned char) * CHUNKS_IN_SINGLE_KERNEL * MAX_FILE_BUFFER_SIZE, NULL, &err);
		lzw_total_bytes_bufs[i] = cl::Buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY,  sizeof(int) * CHUNKS_IN_SINGLE_KERNEL, NULL, &err);
	}

	unsigned char* lzw_chunks[NUM_KERNELS];
	int *lzw_chunks_length[NUM_KERNELS];
    unsigned char* lzw_file_buffer[NUM_KERNELS];
	int *lzw_total_bytes[NUM_KERNELS];

	for(int i = 0; i < NUM_KERNELS; i++){
		lzw_chunks[i] = (unsigned char *)q.enqueueMapBuffer(lzw_chunks_bufs[i], CL_TRUE, CL_MAP_WRITE, 0, sizeof(unsigned char) * CHUNKS_IN_SINGLE_KERNEL * MAX_CHUNK_SIZE);
		lzw_chunks_length[i] = (int*)q.enqueueMapBuffer(lzw_chunks_length_bufs[i], CL_TRUE, CL_MAP_WRITE, 0, sizeof(int) * CHUNKS_IN_SINGLE_KERNEL);
		lzw_file_buffer[i] = (unsigned char*)q.enqueueMapBuffer(lzw_file_buffer_bufs[i], CL_TRUE, CL_MAP_READ, 0, sizeof(unsigned char) * CHUNKS_IN_SINGLE_KERNEL * MAX_FILE_BUFFER_SIZE);
		lzw_total_bytes[i] = (int*)q.enqueueMapBuffer(lzw_total_bytes_bufs[i], CL_TRUE, CL_MAP_READ, 0, sizeof(int) * CHUNKS_IN_SINGLE_KERNEL);
	}

    timer.add("Populating buffer inputs");

// ------------------------------------------------------------------------------------
// Step 3: Run the kernel
// ------------------------------------------------------------------------------------
	std::cout << "dec 9 0105" << std::endl;
	std::cout << argv[1] << std::endl;
	stopwatch ethernet_timer;
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

	writer = 0;

	// initialize the map for deduplication
	std::unordered_map<std::string, int> chunks_map;
	float sum_lzw_raw_length = 0, sum_lzw_cmprs_len = 0, sum_dedup_raw_length = 0, sum_dedup_cmprs_len = 0;

	int base = 0;

	int ticks = 0;

	stopwatch total_time;
	stopwatch cdc_time;
	stopwatch sha_time;
	stopwatch dedup_time;

	int total_inputBits=0;
	int dedup_chars = 0;

	//last message
	while (!done) {
		ticks++;
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

		total_inputBits+=length;
		//printf("length: %d offset %d\n",length,offset);
		memcpy(&file[offset], &buffer[HEADER], length);

		offset += length;
		writer++;

		// encode the obtained information in buffer

		// initialize the vector to store the obtained chunks
		std::vector<std::string> chunks;

		total_time.start();
		cdc_time.start();
		cdc(&buffer[2], chunks, length);
		cdc_time.stop();

		std::vector<bool> lzw_or_dedup; //0 denotes lzw, while 1 denotes dedup

		int lzw_offset = 0;

		std::vector<std::array<unsigned char, 4>> dedup_file_buffer;
		int dedup_offset = 0;

		//calculate hash value and chunk id for each chunk
		//add those key-value pairs to chunks map
		//Question: do we need to consider the situation that different chunks share the same hash value calculated by SHA at this point
		for(std::vector<std::string>::size_type i = 0; i < chunks.size(); i++){
		// for(std::vector<std::string>::size_type i = 0; i < 10; i++){
			hash_part hash_value;
			sha_time.start();
			sha(chunks[i], hash_value);
			sha_time.stop();

			std::string hash_hex_string = toHexString(hash_value);
			
			if(chunks_map.find(hash_hex_string) == chunks_map.end()){
				chunks_map.insert({hash_hex_string, base + i});
				unsigned char* chunk_content = (unsigned char*)malloc(sizeof(unsigned char) * MAX_CHUNK_SIZE);
                convert_string_char(chunks[i], chunk_content);
                int chunk_len = chunks[i].length();
				int total_bytes = 0;
				
				//running kernel on FPGA
				memcpy(&lzw_chunks[lzw_offset % NUM_KERNELS][(lzw_offset / NUM_KERNELS) * MAX_CHUNK_SIZE], chunk_content, chunks[i].length());
				lzw_chunks_length[lzw_offset % NUM_KERNELS][(lzw_offset / NUM_KERNELS)] = chunk_len;

				free(chunk_content);

				lzw_or_dedup.push_back(false);
				lzw_offset++;

				if(lzw_offset == NUM_KERNELS * CHUNKS_IN_SINGLE_KERNEL){
					std::cout << "calculating lzw in multi chunks" << std::endl;
					lzw_offset = 0;
 					// running kernel on FPGA
					printf("Set kernel arguments\n");  
					// Map buffers to kernel arguments, thereby assigning them to specific device memory banks

					for(int k = 0; k < NUM_KERNELS; k++){
						multi_kernels[k].setArg(0, lzw_chunks_bufs[k]);
						multi_kernels[k].setArg(1, lzw_chunks_length_bufs[k]);
						multi_kernels[k].setArg(2, lzw_file_buffer_bufs[k]);
						multi_kernels[k].setArg(3, lzw_total_bytes_bufs[k]);
					}

					printf("Memory object migration enqueue host->device\n");

					std::vector<cl::Event> write_events[NUM_KERNELS];
					std::vector<cl::Event> exec_events[NUM_KERNELS];

					cl::Event write_ev[NUM_KERNELS];
					cl::Event exec_ev[NUM_KERNELS];
					cl::Event read_ev[NUM_KERNELS];

					for(int k = 0; k < NUM_KERNELS; k++){
						q.enqueueMigrateMemObjects({lzw_chunks_bufs[k], lzw_chunks_length_bufs[k]}, 0 /* 0 means from host*/, NULL, &write_ev[k]);
						write_events[k].push_back(write_ev[k]); 	
					}

					printf("enqueue task\n");
					for(int k = 0; k < NUM_KERNELS; k++){
						q.enqueueTask(multi_kernels[k], &write_events[k], &exec_ev[k]);
						exec_events[k].push_back(exec_ev[k]);
					}

					printf("Memory object migration enqueue host->device\n");
					for(int k = 0; k < NUM_KERNELS; k++){
						q.enqueueMigrateMemObjects({lzw_file_buffer_bufs[k], lzw_total_bytes_bufs[k]}, CL_MIGRATE_MEM_OBJECT_HOST, &exec_events[k], &read_ev[k]);
					} 
					q.finish();

					for(int i = 0; i < lzw_or_dedup.size(); i++){
						if(!lzw_or_dedup[i]){
							write_file(&lzw_file_buffer[lzw_offset % NUM_KERNELS][(lzw_offset / NUM_KERNELS) * MAX_FILE_BUFFER_SIZE], lzw_total_bytes[lzw_offset % NUM_KERNELS][(lzw_offset / NUM_KERNELS)], "encoded.bin");
							lzw_offset++;
						}
						else{
							unsigned char write_temp[4];
							std::memcpy(write_temp, dedup_file_buffer[dedup_offset].data(), 4);
							write_file(write_temp, 0, "encoded.bin");
							dedup_offset++;
						}
					}
					lzw_offset = 0;
					dedup_offset = 0;
					lzw_or_dedup.clear();
					dedup_file_buffer.clear();
				}
				total_time.stop();
				sum_lzw_raw_length += chunks[i].length() * 1.5;
				sum_lzw_cmprs_len += total_bytes;
				total_time.start();
			}

			else{
				uint32_t out_code;
				dedup_chars += chunks[i].length();
				dedup_time.start();
				unsigned char* dedup_file_buffer_temp = (unsigned char*)malloc(sizeof(unsigned char) * 4);
				std::array<unsigned char, 4> dedup_arr_temp;
				duplicate_encoding(chunks_map.at(hash_hex_string), out_code, dedup_file_buffer_temp);
				std::memcpy(dedup_arr_temp.data(), dedup_file_buffer_temp, 4);
				free(dedup_file_buffer_temp);
				dedup_file_buffer.push_back(dedup_arr_temp);
				lzw_or_dedup.push_back(true);
				dedup_time.stop();
				total_time.stop();
				sum_dedup_raw_length += chunks[i].length() * 1.5;
				sum_dedup_cmprs_len += 4;
				total_time.start();
			}
		}
		if(lzw_offset != 0){
			std::cout << "calculating lzw in multi chunks" << std::endl;
			lzw_offset = 0;

			// running kernel on FPGA
			timer.add("Set kernel arguments");  
			// Map buffers to kernel arguments, thereby assigning them to specific device memory banks

			for(int k = 0; k < NUM_KERNELS; k++){
				multi_kernels[k].setArg(0, lzw_chunks_bufs[k]);
				multi_kernels[k].setArg(1, lzw_chunks_length_bufs[k]);
				multi_kernels[k].setArg(2, lzw_file_buffer_bufs[k]);
				multi_kernels[k].setArg(3, lzw_total_bytes_bufs[k]);
			}

			timer.add("Memory object migration enqueue host->device");

			std::vector<cl::Event> write_events[NUM_KERNELS];
			std::vector<cl::Event> exec_events[NUM_KERNELS];

			cl::Event write_ev[NUM_KERNELS];
			cl::Event exec_ev[NUM_KERNELS];
			cl::Event read_ev[NUM_KERNELS];

			for(int k = 0; k < NUM_KERNELS; k++){
				q.enqueueMigrateMemObjects({lzw_chunks_bufs[k], lzw_chunks_length_bufs[k]}, 0 /* 0 means from host*/, NULL, &write_ev[k]);
				write_events[k].push_back(write_ev[k]); 	
			}

			for(int k = 0; k < NUM_KERNELS; k++){
				q.enqueueTask(multi_kernels[k], &write_events[k], &exec_ev[k]);
				exec_events[k].push_back(exec_ev[k]);
			}

			for(int k = 0; k < NUM_KERNELS; k++){
				q.enqueueMigrateMemObjects({lzw_file_buffer_bufs[k], lzw_total_bytes_bufs[k]}, CL_MIGRATE_MEM_OBJECT_HOST, &exec_events[k], &read_ev[k]);
			} 
			q.finish();

			for(int i = 0; i < lzw_or_dedup.size(); i++){
				if(!lzw_or_dedup[i]){
					write_file(&lzw_file_buffer[lzw_offset % NUM_KERNELS][(lzw_offset / NUM_KERNELS) * MAX_FILE_BUFFER_SIZE], lzw_total_bytes[lzw_offset % NUM_KERNELS][(lzw_offset / NUM_KERNELS)], "encoded.bin");
					lzw_offset++;
				}
				else{
					unsigned char write_temp[4];
					std::memcpy(write_temp, dedup_file_buffer[dedup_offset].data(), 4);
					write_file(write_temp, 0, "encoded.bin");
					dedup_offset++;
				}
			}
			lzw_offset = 0;
			dedup_offset = 0;
			lzw_or_dedup.clear();
			dedup_file_buffer.clear();
		}
		base += chunks.size();
	}

	total_time.stop();

	float lzw_compress_ratio = sum_lzw_raw_length / sum_lzw_cmprs_len;
	float dedup_compress_ratio = sum_dedup_raw_length / sum_dedup_cmprs_len;
	float total_compress_ratio = (sum_dedup_raw_length + sum_lzw_raw_length) / (sum_dedup_cmprs_len + sum_lzw_cmprs_len);

	std::cout << "LZW compress ratio: " << lzw_compress_ratio << std::endl;
	std::cout << "Deduplication compress ratio: " << dedup_compress_ratio << std::endl;
	std::cout << "Total compress ratio: " << total_compress_ratio << std::endl;

	std::cout << "Loop times: " << ticks << std::endl;

	FILE *outfd = fopen("output_cpu.bin", "wb");
	int bytes_written = fwrite(&file[0], 1, offset, outfd);
	printf("write file with %d\n", bytes_written);
	fclose(outfd);

	for (int i = 0; i < NUM_PACKETS; i++) {
		free(input[i]);
	}

	free(file);
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
	for(int k = 0; k < NUM_KERNELS; k++){
		q.enqueueUnmapMemObject(lzw_chunks_bufs[k], lzw_chunks[k]);
		q.enqueueUnmapMemObject(lzw_chunks_length_bufs[k], lzw_chunks_length[k]);
		q.enqueueUnmapMemObject(lzw_file_buffer_bufs[k], lzw_file_buffer[k]);
		q.enqueueUnmapMemObject(lzw_total_bytes_bufs[k], lzw_total_bytes[k]);
	}
    q.finish();

    // std::cout << "--------------- Key execution times ---------------" << std::endl;
    // timer.print();

    // std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl; 
    // return (match ? EXIT_SUCCESS : EXIT_FAILURE);
    return 0;
}