
#include "lzw_encode.h"

int compare_match(int dict[MAX_DICT_SIZE][256], uint8_t curr_string[], int curr_string_len, int dict_index){
    for (int i = 256; i <= dict_index; i++) { 
        int match = 1;
        for (int j = 0; j < curr_string_len; j++) {
            if (dict[i][j] != curr_string[j]) {
                match = 0;
                break;
            }
        }
        if (match) {
            return i; 
        }
    }
    return -1; 
}

void lzw(uint8_t input[], int size, int dict[MAX_DICT_SIZE][256], uint16_t output[], int& output_index){    
    int dict_index = 256; // if add new, start from index 256
    
    for(int i = 0; i < size; ++i){
        //corner case last byte
        if(i == size - 1) {
            output[output_index++] = input[i];
        }

        uint8_t curr_string[256]; // track current string
        int curr_string_len = 0; // start from nothing
        int curr_code = input[i]; //value to be sent
        curr_string[curr_string_len] = input[i]; //temp store in curr_string
        curr_string_len++;
        int temp = -1;
        while(i + curr_string_len < size){
            curr_string[curr_string_len] = input[i + curr_string_len]; //temp store in curr_string
            curr_string_len++;

            temp = compare_match(dict, curr_string, curr_string_len, dict_index);
            if(temp == -1){ // didn't find
                //add new to dict
                for(int j = 0; j < curr_string_len; ++j){
                    dict[dict_index][j] = curr_string[j];
                }
                i =  i + curr_string_len - 2;
                dict_index++;
                output[output_index] = curr_code;
                output_index++;
                break;
            } 
            else { //find in dict
                curr_code = temp; 
            }

        }
    }

}

void init_dict(int dict[MAX_DICT_SIZE][256]){
    for(int i = 0; i < 256; i++){
        dict[i][0] = i;
    }
}

void convert_output(uint16_t in[], uint8_t out[], int size){
    //header
    out[0] = size >> 23;
    out[1] = size >> 15;
    out[2] = size >> 7;
    out[3] = ((size & 0x7f) << 1);

    //data
    int index = 5;
    
    out[4] = in[0] >> 5;

    for(int i = 0; i < size - 1; i++){
        out[index++] = in[i] << 3 | in[i + 1] >> 10;
        out[index++] = in[i + 1] >> 5;
    }

    out[index] = in[size - 1] << 3;
}

// int main() {
//     uint8_t input[8] = {'A','B','A','B','A','A','C','A'}; 
//     uint16_t output[100];
//     uint8_t real_output[200];
//     int dict[MAX_DICT_SIZE][256];
//     int output_index = 0;

//     memset(dict, 0, sizeof(dict));
//     memset(output, 0, sizeof(output));

//     init_dict(dict);

//     lzw(input, 8, dict, output, output_index);

//     convert_output(output, real_output, output_index);

//     // for(int i = 0; i < 8; ++i) {
//     //     printf("%d ", output[i]);
//     // }

//     return 0;
// } 

    // int dict[4096][256];
    // uint16_t output[4096];
    // int output_index = 0;
    // init_dict(dict);
    // lzw(chunks[1], chunk_sizes[1], output, output_index);

    // for(int i = 0; i < output_index; i++) {
    //     printf("encode : %d \n", output[i]); 
    // }