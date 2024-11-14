
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
        while(i + curr_string_len <= size){
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

int convert_output(uint16_t in[], uint8_t out[], int input_size){
    //header
    int output_size = 0;

    int adjusted_input_size = input_size - (input_size % 2);

    for(int i = 0; i < adjusted_input_size; i+=2){
        //printf("in[i]: %hu\n", in[i]);
        //printf("in[i+1]: %hu\n", in[i+1]);
        

        out[output_size] = (in[i]>>4) & 0xff;
        //printf("out[1]: %u\n", static_cast<unsigned int>(out[output_size]));
        output_size++;
        out[output_size] = ((in[i] << 4) & 0xf0) | ((in[i+1] >> 8) & 0x0f);
        //printf("out[2]: %u\n", static_cast<unsigned int>(out[output_size]));
        output_size++;
        out[output_size] = (in[i+1]) & 0xff;
        //printf("out[3]: %u\n", static_cast<unsigned int>(out[output_size]));
        output_size++;
    }

    if (input_size % 2 != 0) {
        out[output_size] = (in[adjusted_input_size] >> 4) & 0xFF;
        output_size++;
        out[output_size] = (in[adjusted_input_size] << 4) & 0xF0;
        output_size++;
    }

    return output_size;
}

