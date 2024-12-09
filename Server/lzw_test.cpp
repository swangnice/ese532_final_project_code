#include "lzw.h"

void main(){
    
    
    unsigned char* s1;
    s1 = (unsigned char*)malloc(sizeof(unsigned char)*2048);
    strcpy((char *)s1, "The Little Prince Chapter I\nOnce when I was six years old I saw a magnificent picture in a book, called True Stories from Nature, about the primeval forest. It was a picture of a boa constrictor in the act of swallowing an animal. Here is a copy of the drawing.\nBoa\nIn the book it");
    int length = strlen((char *)s1);
    int is_dup = 0;
    int dup_index = 0;
    uint8_t *temp_out_buffer;
    temp_out_buffer = (uint8_t*)malloc(sizeof(uint8_t)*2048);
    unsigned int temp_out_buffer_size = 0;
    
    lzw_compress_v10086(s1, &length, &is_dup, &dup_index, temp_out_buffer, &temp_out_buffer_size);
    free(s1);
}