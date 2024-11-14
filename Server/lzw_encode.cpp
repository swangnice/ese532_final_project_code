
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

// int main() {
//     uint8_t text[] = {
//         'T', 'h', 'e', ' ', 'L', 'i', 't', 't', 'l', 'e', ' ', 'P', 'r', 'i', 'n', 'c', 'e', ' ', 
//         'C', 'h', 'a', 'p', 't', 'e', 'r', ' ', 'I', '\n',
//         'O', 'n', 'c', 'e', ' ', 'w', 'h', 'e', 'n', ' ', 'I', ' ', 'w', 'a', 's', ' ', 's', 'i', 'x', 
//         ' ', 'y', 'e', 'a', 'r', 's', ' ', 'o', 'l', 'd', ' ', 'I', ' ', 's', 'a', 'w', ' ', 'a', ' ', 
//         'm', 'a', 'g', 'n', 'i', 'f', 'i', 'c', 'e', 'n', 't', ' ', 'p', 'i', 'c', 't', 'u', 'r', 'e', 
//         ' ', 'i', 'n', ' ', 'a', ' ', 'b', 'o', 'o', 'k', ',', ' ', 'c', 'a', 'l', 'l', 'e', 'd', ' ', 
//         'T', 'r', 'u', 'e', ' ', 'S', 't', 'o', 'r', 'i', 'e', 's', ' ', 'f', 'r', 'o', 'm', ' ', 'N', 
//         'a', 't', 'u', 'r', 'e', ',', ' ', 'a', 'b', 'o', 'u', 't', ' ', 't', 'h', 'e', ' ', 'p', 'r', 
//         'i', 'm', 'e', 'v', 'a', 'l', ' ', 'f', 'o', 'r', 'e', 's', 't', '.', ' ', 'I', 't', ' ', 'w', 
//         'a', 's', ' ', 'a', ' ', 'p', 'i', 'c', 't', 'u', 'r', 'e', ' ', 'o', 'f', ' ', 'a', ' ', 'b', 
//         'o', 'a', ' ', 'c', 'o', 'n', 's', 't', 'r', 'i', 'c', 't', 'o', 'r', ' ', 'i', 'n', ' ', 't', 
//         'h', 'e', ' ', 'a', 'c', 't', ' ', 'o', 'f', ' ', 's', 'w', 'a', 'l', 'l', 'o', 'w', 'i', 'n', 
//         'g', ' ', 'a', 'n', ' ', 'a', 'n', 'i', 'm', 'a', 'l', '.', ' ', 'H', 'e', 'r', 'e', ' ', 'i', 
//         's', ' ', 'a', ' ', 'c', 'o', 'p', 'y', ' ', 'o', 'f', ' ', 't', 'h', 'e', ' ', 'd', 'r', 'a', 
//         'w', 'i', 'n', 'g', '.', '\n',
//         'B', 'o', 'a', '\n',
//         'I', 'n', ' ', 't', 'h', 'e', ' ', 'b', 'o', 'o', 'k', ' ', 'i', 't', ' ', 's', 'a', 'i', 'd', 
//         ':', ' ', '"', 'B', 'o', 'a', ' ', 'c', 'o', 'n', 's', 't', 'r', 'i', 'c', 't', 'o', 'r', 's', 
//         ' ', 's', 'w', 'a', 'l', 'l', 'o', 'w', ' ', 't', 'h', 'e', 'i', 'r', ' ', 'p', 'r', 'e', 'y', 
//         ' ', 'w', 'h', 'o', 'l', 'e', ',', ' ', 'w', 'i', 't', 'h', 'o', 'u', 't', ' ', 'c', 'h', 'e', 
//         'w', 'i', 'n', 'g', ' ', 'i', 't', '.', ' ', 'A', 'f', 't', 'e', 'r', ' ', 't', 'h', 'a', 't', 
//         ' ', 't', 'h', 'e', 'y', ' ', 'a', 'r', 'e', ' ', 'n', 'o', 't', ' ', 'a', 'b', 'l', 'e', ' ', 
//         't', 'o', ' ', 'm', 'o', 'v', 'e', ',', ' ', 'a', 'n', 'd', ' ', 't', 'h', 'e', 'y', ' ', 's', 
//         'l', 'e', 'e', 'p', ' ', 't', 'h', 'r', 'o', 'u', 'g', 'h', ' ', 't', 'h', 'e', ' ', 's', 'i', 
//         'x', ' ', 'm', 'o', 'n', 't', 'h', 's', ' ', 't', 'h', 'a', 't', ' ', 't', 'h', 'e', 'y', ' ', 
//         'n', 'e', 'e', 'd', ' ', 'f', 'o', 'r', ' ', 'd', 'i', 'g', 'e', 's', 't', 'i', 'o', 'n', '.', 
//         '"', '\n',
//         'I', ' ', 'p', 'o', 'n', 'd', 'e', 'r', 'e', 'd', ' ', 'd', 'e', 'e', 'p', 'l', 'y', ',', ' ', 
//         't', 'h', 'e', 'n', ',', ' ', 'o', 'v', 'e', 'r', ' ', 't', 'h', 'e', ' ', 'a', 'd', 'v', 'e', 
//         'n', 't', 'u', 'r', 'e', 's', ' ', 'o', 'f', ' ', 't', 'h', 'e', ' ', 'j', 'u', 'n', 'g', 'l', 
//         'e', '.', ' ', 'A', 'n', 'd', ' ', 'a', 'f', 't', 'e', 'r', ' ', 's', 'o', 'm', 'e', ' ', 'w', 
//         'o', 'r', 'k', ' ', 'w', 'i', 't', 'h', ' ', 'a', ' ', 'c', 'o', 'l', 'o', 'r', 'e', 'd', ' ', 
//         'p', 'e', 'n', 'c', 'i', 'l', ' ', 'I', ' ', 's', 'u', 'c', 'c', 'e', 'e', 'd', 'e', 'd', ' ', 
//         'i', 'n', ' ', 'm', 'a', 'k', 'i', 'n', 'g', ' ', 'm', 'y', ' ', 'f', 'i', 'r', 's', 't', ' ', 
//         'd', 'r', 'a', 'w', 'i', 'n', 'g', '.', ' ', 'M', 'y', ' ', 'D', 'r', 'a', 'w', 'i', 'n', 'g', 
//         ' ', 'N', 'u', 'm', 'b', 'e', 'r', ' ', 'O', 'n', 'e', '.', ' ', 'I', 't', ' ', 'l', 'o', 'o', 
//         'k', 'e', 'd', ' ', 's', 'o', 'm', 'e', 't', 'h', 'i', 'n', 'g', ' ', 'l', 'i', 'k', 'e', ' ', 
//         't', 'h', 'i', 's', ':', '\n',
//         'H', 'a', 't', '\n',
//         'I', ' ', 's', 'h', 'o', 'w', 'e', 'd', ' ', 'm', 'y', ' ', 'm', 'a', 's', 't', 'e', 'r', 'p', 
//         'i', 'e', 'c', 'e', ' ', 't', 'o', ' ', 't', 'h', 'e', ' ', 'g', 'r', 'o', 'w', 'n', '-', 'u', 
//         'p', 's', ',', ' ', 'a', 'n', 'd', ' ', 'a', 's', 'k', 'e', 'd', ' ', 't', 'h', 'e', 'm', ' ', 
//         'w', 'h', 'e', 't', 'h', 'e', 'r', ' ', 't', 'h', 'e', ' ', 'd', 'r', 'a', 'w', 'i', 'n', 'g', 
//         ' ', 'f', 'r', 'i', 'g', 'h', 't', 'e', 'n', 'e', 'd', ' ', 't', 'h', 'e', 'm', '.', '\n',
//         'B', 'u', 't', ' ', 't', 'h', 'e', 'y', ' ', 'a', 'n', 's', 'w', 'e', 'r', 'e', 'd', ':', ' ', 
//         '"', 'F', 'r', 'i', 'g', 'h', 't', 'e', 'n', '?', ' ', 'W', 'h', 'y', ' ', 's', 'h', 'o', 'u', 
//         'l', 'd', ' ', 'a', 'n', 'y', ' ', 'o', 'n', 'e', ' ', 'b', 'e', ' ', 'f', 'r', 'i', 'g', 'h', 
//         't', 'e', 'n', 'e', 'd', ' ', 'b', 'y', ' ', 'a', ' ', 'h', 'a', 't', '?', '"', '\n',
//         'M', 'y', ' ', 'd', 'r', 'a', 'w', 'i', 'n', 'g', ' ', 'w', 'a', 's', ' ', 'n', 'o', 't', ' ', 
//         'a', ' ', 'p', 'i', 'c', 't', 'u', 'r', 'e', ' ', 'o', 'f', ' ', 'a', ' ', 'h', 'a', 't', '.', 
//         ' ', 'I', 't', ' ', 'w', 'a', 's', ' ', 'a', ' ', 'p', 'i', 'c', 't', 'u', 'r', 'e', ' ', 'o', 
//         'f', ' ', 'a', ' ', 'b', 'o', 'a', ' ', 'c', 'o', 'n', 's', 't', 'r', 'i', 'c', 't', 'o', 'r', 
//         ' ', 'd', 'i', 'g', 'e', 's', 't', 'i', 'n', 'g', ' ', 'a', 'n', ' ', 'e', 'l', 'e', 'p', 'h', 
//         'a', 'n', 't', '.', ' ', 'B', 'u', 't', ' ', 's', 'i', 'n', 'c', 'e', ' ', 't', 'h', 'e', ' ', 
//         'g', 'r', 'o', 'w', 'n', '-', 'u', 'p', 's', ' ', 'w', 'e', 'r', 'e', ' ', 'n', 'o', 't', ' ', 
//         'a', 'b', 'l', 'e', ' ', 't', 'o', ' ', 'u', 'n', 'd', 'e', 'r', 's', 't', 'a', 'n', 'd', ' ', 
//         'i', 't', ',', ' ', 'I', ' ', 'm', 'a', 'd', 'e', ' ', 'a', 'n', 'o', 't', 'h', 'e', 'r', ' ', 
//         'd', 'r', 'a', 'w', 'i', 'n', 'g', ':', ' ', 'I', ' ', 'd', 'r', 'e', 'w', ' ', 't', 'h', 'e'
//     };

//     uint16_t output[5000];
//     int dict[MAX_DICT_SIZE][256];
//     int output_index = 0;

//     memset(dict, 0, sizeof(dict));
//     memset(output, 0, sizeof(output));

//     init_dict(dict);

//     lzw(text, sizeof(text), dict, output, output_index);

//     //convert_output(output, real_output, output_index);

//     // for(int i = 0; i < 8; ++i) {
//     //     printf("%d ", output[i]);
//     // }

//     for(int i = 0; i < output_index; i++){
//         printf("%d ", output[i]);
//     }

//     return 0;
// } 

