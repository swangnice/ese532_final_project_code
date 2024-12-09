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
extern "C" void lzw_compress(int*, int, int, int, int);
extern "C" void apatb_lzw_compress_hw(volatile void * __xlx_apatb_param_s1, volatile void * __xlx_apatb_param_length, volatile void * __xlx_apatb_param_out_code, volatile void * __xlx_apatb_param_out_len) {
  // Collect __xlx_s1_length_out_code_out_len__tmp_vec
  vector<sc_bv<32> >__xlx_s1_length_out_code_out_len__tmp_vec;
  for (int j = 0, e = 1; j != e; ++j) {
    __xlx_s1_length_out_code_out_len__tmp_vec.push_back(((int*)__xlx_apatb_param_s1)[j]);
  }
  int __xlx_size_param_s1 = 1;
  int __xlx_offset_param_s1 = 0;
  int __xlx_offset_byte_param_s1 = 0*4;
  for (int j = 0, e = 1; j != e; ++j) {
    __xlx_s1_length_out_code_out_len__tmp_vec.push_back(((int*)__xlx_apatb_param_length)[j]);
  }
  int __xlx_size_param_length = 1;
  int __xlx_offset_param_length = 1;
  int __xlx_offset_byte_param_length = 1*4;
  for (int j = 0, e = 1; j != e; ++j) {
    __xlx_s1_length_out_code_out_len__tmp_vec.push_back(((int*)__xlx_apatb_param_out_code)[j]);
  }
  int __xlx_size_param_out_code = 1;
  int __xlx_offset_param_out_code = 2;
  int __xlx_offset_byte_param_out_code = 2*4;
  for (int j = 0, e = 1; j != e; ++j) {
    __xlx_s1_length_out_code_out_len__tmp_vec.push_back(((int*)__xlx_apatb_param_out_len)[j]);
  }
  int __xlx_size_param_out_len = 1;
  int __xlx_offset_param_out_len = 3;
  int __xlx_offset_byte_param_out_len = 3*4;
  int* __xlx_s1_length_out_code_out_len__input_buffer= new int[__xlx_s1_length_out_code_out_len__tmp_vec.size()];
  for (int i = 0; i < __xlx_s1_length_out_code_out_len__tmp_vec.size(); ++i) {
    __xlx_s1_length_out_code_out_len__input_buffer[i] = __xlx_s1_length_out_code_out_len__tmp_vec[i].range(31, 0).to_uint64();
  }
  // DUT call
  lzw_compress(__xlx_s1_length_out_code_out_len__input_buffer, __xlx_offset_byte_param_s1, __xlx_offset_byte_param_length, __xlx_offset_byte_param_out_code, __xlx_offset_byte_param_out_len);
// print __xlx_apatb_param_s1
  sc_bv<32>*__xlx_s1_output_buffer = new sc_bv<32>[__xlx_size_param_s1];
  for (int i = 0; i < __xlx_size_param_s1; ++i) {
    __xlx_s1_output_buffer[i] = __xlx_s1_length_out_code_out_len__input_buffer[i+__xlx_offset_param_s1];
  }
  for (int i = 0; i < __xlx_size_param_s1; ++i) {
    ((int*)__xlx_apatb_param_s1)[i] = __xlx_s1_output_buffer[i].to_uint64();
  }
// print __xlx_apatb_param_length
  sc_bv<32>*__xlx_length_output_buffer = new sc_bv<32>[__xlx_size_param_length];
  for (int i = 0; i < __xlx_size_param_length; ++i) {
    __xlx_length_output_buffer[i] = __xlx_s1_length_out_code_out_len__input_buffer[i+__xlx_offset_param_length];
  }
  for (int i = 0; i < __xlx_size_param_length; ++i) {
    ((int*)__xlx_apatb_param_length)[i] = __xlx_length_output_buffer[i].to_uint64();
  }
// print __xlx_apatb_param_out_code
  sc_bv<32>*__xlx_out_code_output_buffer = new sc_bv<32>[__xlx_size_param_out_code];
  for (int i = 0; i < __xlx_size_param_out_code; ++i) {
    __xlx_out_code_output_buffer[i] = __xlx_s1_length_out_code_out_len__input_buffer[i+__xlx_offset_param_out_code];
  }
  for (int i = 0; i < __xlx_size_param_out_code; ++i) {
    ((int*)__xlx_apatb_param_out_code)[i] = __xlx_out_code_output_buffer[i].to_uint64();
  }
// print __xlx_apatb_param_out_len
  sc_bv<32>*__xlx_out_len_output_buffer = new sc_bv<32>[__xlx_size_param_out_len];
  for (int i = 0; i < __xlx_size_param_out_len; ++i) {
    __xlx_out_len_output_buffer[i] = __xlx_s1_length_out_code_out_len__input_buffer[i+__xlx_offset_param_out_len];
  }
  for (int i = 0; i < __xlx_size_param_out_len; ++i) {
    ((int*)__xlx_apatb_param_out_len)[i] = __xlx_out_len_output_buffer[i].to_uint64();
  }
}
