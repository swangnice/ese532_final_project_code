#include <systemc>
#include <vector>
#include <iostream>
#include "hls_stream.h"
#include "ap_int.h"
#include "ap_fixed.h"
using namespace std;
using namespace sc_dt;
class AESL_RUNTIME_BC {
  public:
    AESL_RUNTIME_BC(const char* name) {
      file_token.open( name);
      if (!file_token.good()) {
        cout << "Failed to open tv file " << name << endl;
        exit (1);
      }
      file_token >> mName;//[[[runtime]]]
    }
    ~AESL_RUNTIME_BC() {
      file_token.close();
    }
    int read_size () {
      int size = 0;
      file_token >> mName;//[[transaction]]
      file_token >> mName;//transaction number
      file_token >> mName;//pop_size
      size = atoi(mName.c_str());
      file_token >> mName;//[[/transaction]]
      return size;
    }
  public:
    fstream file_token;
    string mName;
};
extern "C" void lzw_compress_hw(int*, int*, int*, int*, int*, int, int, int, int, int, int);
extern "C" void apatb_lzw_compress_hw_hw(volatile void * __xlx_apatb_param_s1, volatile void * __xlx_apatb_param_length, volatile void * __xlx_apatb_param_is_dup, volatile void * __xlx_apatb_param_dup_index, volatile void * __xlx_apatb_param_temp_out_buffer, volatile void * __xlx_apatb_param_temp_out_buffer_size) {
  // Collect __xlx_s1_temp_out_buffer__tmp_vec
  vector<sc_bv<32> >__xlx_s1_temp_out_buffer__tmp_vec;
  for (int j = 0, e = 512; j != e; ++j) {
    __xlx_s1_temp_out_buffer__tmp_vec.push_back(((int*)__xlx_apatb_param_s1)[j]);
  }
  int __xlx_size_param_s1 = 512;
  int __xlx_offset_param_s1 = 0;
  int __xlx_offset_byte_param_s1 = 0*4;
  for (int j = 0, e = 512; j != e; ++j) {
    __xlx_s1_temp_out_buffer__tmp_vec.push_back(((int*)__xlx_apatb_param_temp_out_buffer)[j]);
  }
  int __xlx_size_param_temp_out_buffer = 512;
  int __xlx_offset_param_temp_out_buffer = 512;
  int __xlx_offset_byte_param_temp_out_buffer = 512*4;
  int* __xlx_s1_temp_out_buffer__input_buffer= new int[__xlx_s1_temp_out_buffer__tmp_vec.size()];
  for (int i = 0; i < __xlx_s1_temp_out_buffer__tmp_vec.size(); ++i) {
    __xlx_s1_temp_out_buffer__input_buffer[i] = __xlx_s1_temp_out_buffer__tmp_vec[i].range(31, 0).to_uint64();
  }
  // Collect __xlx_length__tmp_vec
  vector<sc_bv<32> >__xlx_length__tmp_vec;
  for (int j = 0, e = 1; j != e; ++j) {
    __xlx_length__tmp_vec.push_back(((int*)__xlx_apatb_param_length)[j]);
  }
  int __xlx_size_param_length = 1;
  int __xlx_offset_param_length = 0;
  int __xlx_offset_byte_param_length = 0*4;
  int* __xlx_length__input_buffer= new int[__xlx_length__tmp_vec.size()];
  for (int i = 0; i < __xlx_length__tmp_vec.size(); ++i) {
    __xlx_length__input_buffer[i] = __xlx_length__tmp_vec[i].range(31, 0).to_uint64();
  }
  // Collect __xlx_is_dup__tmp_vec
  vector<sc_bv<32> >__xlx_is_dup__tmp_vec;
  for (int j = 0, e = 1; j != e; ++j) {
    __xlx_is_dup__tmp_vec.push_back(((int*)__xlx_apatb_param_is_dup)[j]);
  }
  int __xlx_size_param_is_dup = 1;
  int __xlx_offset_param_is_dup = 0;
  int __xlx_offset_byte_param_is_dup = 0*4;
  int* __xlx_is_dup__input_buffer= new int[__xlx_is_dup__tmp_vec.size()];
  for (int i = 0; i < __xlx_is_dup__tmp_vec.size(); ++i) {
    __xlx_is_dup__input_buffer[i] = __xlx_is_dup__tmp_vec[i].range(31, 0).to_uint64();
  }
  // Collect __xlx_dup_index__tmp_vec
  vector<sc_bv<32> >__xlx_dup_index__tmp_vec;
  for (int j = 0, e = 1; j != e; ++j) {
    __xlx_dup_index__tmp_vec.push_back(((int*)__xlx_apatb_param_dup_index)[j]);
  }
  int __xlx_size_param_dup_index = 1;
  int __xlx_offset_param_dup_index = 0;
  int __xlx_offset_byte_param_dup_index = 0*4;
  int* __xlx_dup_index__input_buffer= new int[__xlx_dup_index__tmp_vec.size()];
  for (int i = 0; i < __xlx_dup_index__tmp_vec.size(); ++i) {
    __xlx_dup_index__input_buffer[i] = __xlx_dup_index__tmp_vec[i].range(31, 0).to_uint64();
  }
  // Collect __xlx_temp_out_buffer_size__tmp_vec
  vector<sc_bv<32> >__xlx_temp_out_buffer_size__tmp_vec;
  for (int j = 0, e = 1; j != e; ++j) {
    __xlx_temp_out_buffer_size__tmp_vec.push_back(((int*)__xlx_apatb_param_temp_out_buffer_size)[j]);
  }
  int __xlx_size_param_temp_out_buffer_size = 1;
  int __xlx_offset_param_temp_out_buffer_size = 0;
  int __xlx_offset_byte_param_temp_out_buffer_size = 0*4;
  int* __xlx_temp_out_buffer_size__input_buffer= new int[__xlx_temp_out_buffer_size__tmp_vec.size()];
  for (int i = 0; i < __xlx_temp_out_buffer_size__tmp_vec.size(); ++i) {
    __xlx_temp_out_buffer_size__input_buffer[i] = __xlx_temp_out_buffer_size__tmp_vec[i].range(31, 0).to_uint64();
  }
  // DUT call
  lzw_compress_hw(__xlx_s1_temp_out_buffer__input_buffer, __xlx_length__input_buffer, __xlx_is_dup__input_buffer, __xlx_dup_index__input_buffer, __xlx_temp_out_buffer_size__input_buffer, __xlx_offset_byte_param_s1, __xlx_offset_byte_param_length, __xlx_offset_byte_param_is_dup, __xlx_offset_byte_param_dup_index, __xlx_offset_byte_param_temp_out_buffer, __xlx_offset_byte_param_temp_out_buffer_size);
// print __xlx_apatb_param_s1
  sc_bv<32>*__xlx_s1_output_buffer = new sc_bv<32>[__xlx_size_param_s1];
  for (int i = 0; i < __xlx_size_param_s1; ++i) {
    __xlx_s1_output_buffer[i] = __xlx_s1_temp_out_buffer__input_buffer[i+__xlx_offset_param_s1];
  }
  for (int i = 0; i < __xlx_size_param_s1; ++i) {
    ((int*)__xlx_apatb_param_s1)[i] = __xlx_s1_output_buffer[i].to_uint64();
  }
// print __xlx_apatb_param_temp_out_buffer
  sc_bv<32>*__xlx_temp_out_buffer_output_buffer = new sc_bv<32>[__xlx_size_param_temp_out_buffer];
  for (int i = 0; i < __xlx_size_param_temp_out_buffer; ++i) {
    __xlx_temp_out_buffer_output_buffer[i] = __xlx_s1_temp_out_buffer__input_buffer[i+__xlx_offset_param_temp_out_buffer];
  }
  for (int i = 0; i < __xlx_size_param_temp_out_buffer; ++i) {
    ((int*)__xlx_apatb_param_temp_out_buffer)[i] = __xlx_temp_out_buffer_output_buffer[i].to_uint64();
  }
// print __xlx_apatb_param_length
  sc_bv<32>*__xlx_length_output_buffer = new sc_bv<32>[__xlx_size_param_length];
  for (int i = 0; i < __xlx_size_param_length; ++i) {
    __xlx_length_output_buffer[i] = __xlx_length__input_buffer[i+__xlx_offset_param_length];
  }
  for (int i = 0; i < __xlx_size_param_length; ++i) {
    ((int*)__xlx_apatb_param_length)[i] = __xlx_length_output_buffer[i].to_uint64();
  }
// print __xlx_apatb_param_is_dup
  sc_bv<32>*__xlx_is_dup_output_buffer = new sc_bv<32>[__xlx_size_param_is_dup];
  for (int i = 0; i < __xlx_size_param_is_dup; ++i) {
    __xlx_is_dup_output_buffer[i] = __xlx_is_dup__input_buffer[i+__xlx_offset_param_is_dup];
  }
  for (int i = 0; i < __xlx_size_param_is_dup; ++i) {
    ((int*)__xlx_apatb_param_is_dup)[i] = __xlx_is_dup_output_buffer[i].to_uint64();
  }
// print __xlx_apatb_param_dup_index
  sc_bv<32>*__xlx_dup_index_output_buffer = new sc_bv<32>[__xlx_size_param_dup_index];
  for (int i = 0; i < __xlx_size_param_dup_index; ++i) {
    __xlx_dup_index_output_buffer[i] = __xlx_dup_index__input_buffer[i+__xlx_offset_param_dup_index];
  }
  for (int i = 0; i < __xlx_size_param_dup_index; ++i) {
    ((int*)__xlx_apatb_param_dup_index)[i] = __xlx_dup_index_output_buffer[i].to_uint64();
  }
// print __xlx_apatb_param_temp_out_buffer_size
  sc_bv<32>*__xlx_temp_out_buffer_size_output_buffer = new sc_bv<32>[__xlx_size_param_temp_out_buffer_size];
  for (int i = 0; i < __xlx_size_param_temp_out_buffer_size; ++i) {
    __xlx_temp_out_buffer_size_output_buffer[i] = __xlx_temp_out_buffer_size__input_buffer[i+__xlx_offset_param_temp_out_buffer_size];
  }
  for (int i = 0; i < __xlx_size_param_temp_out_buffer_size; ++i) {
    ((int*)__xlx_apatb_param_temp_out_buffer_size)[i] = __xlx_temp_out_buffer_size_output_buffer[i].to_uint64();
  }
}
