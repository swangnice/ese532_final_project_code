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
struct __cosim_s40__ { char data[64]; };
extern "C" void lzw_compress_hw(__cosim_s40__*, int, int, int, int, int, int);
extern "C" void apatb_lzw_compress_hw_hw(volatile void * __xlx_apatb_param_s1, volatile void * __xlx_apatb_param_length, volatile void * __xlx_apatb_param_is_dup, volatile void * __xlx_apatb_param_dup_index, volatile void * __xlx_apatb_param_temp_out_buffer, volatile void * __xlx_apatb_param_temp_out_buffer_size) {
  // Collect __xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__tmp_vec
  vector<sc_bv<512> >__xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__tmp_vec;
  for (int j = 0, e = 1; j != e; ++j) {
    sc_bv<512> _xlx_tmp_sc;
    _xlx_tmp_sc.range(63, 0) = ((long long*)__xlx_apatb_param_s1)[j*8+0];
    _xlx_tmp_sc.range(127, 64) = ((long long*)__xlx_apatb_param_s1)[j*8+1];
    _xlx_tmp_sc.range(191, 128) = ((long long*)__xlx_apatb_param_s1)[j*8+2];
    _xlx_tmp_sc.range(255, 192) = ((long long*)__xlx_apatb_param_s1)[j*8+3];
    _xlx_tmp_sc.range(319, 256) = ((long long*)__xlx_apatb_param_s1)[j*8+4];
    _xlx_tmp_sc.range(383, 320) = ((long long*)__xlx_apatb_param_s1)[j*8+5];
    _xlx_tmp_sc.range(447, 384) = ((long long*)__xlx_apatb_param_s1)[j*8+6];
    _xlx_tmp_sc.range(511, 448) = ((long long*)__xlx_apatb_param_s1)[j*8+7];
    __xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__tmp_vec.push_back(_xlx_tmp_sc);
  }
  int __xlx_size_param_s1 = 1;
  int __xlx_offset_param_s1 = 0;
  int __xlx_offset_byte_param_s1 = 0*64;
  for (int j = 0, e = 1; j != e; ++j) {
    sc_bv<512> _xlx_tmp_sc;
    _xlx_tmp_sc.range(63, 0) = ((long long*)__xlx_apatb_param_length)[j*8+0];
    _xlx_tmp_sc.range(127, 64) = ((long long*)__xlx_apatb_param_length)[j*8+1];
    _xlx_tmp_sc.range(191, 128) = ((long long*)__xlx_apatb_param_length)[j*8+2];
    _xlx_tmp_sc.range(255, 192) = ((long long*)__xlx_apatb_param_length)[j*8+3];
    _xlx_tmp_sc.range(319, 256) = ((long long*)__xlx_apatb_param_length)[j*8+4];
    _xlx_tmp_sc.range(383, 320) = ((long long*)__xlx_apatb_param_length)[j*8+5];
    _xlx_tmp_sc.range(447, 384) = ((long long*)__xlx_apatb_param_length)[j*8+6];
    _xlx_tmp_sc.range(511, 448) = ((long long*)__xlx_apatb_param_length)[j*8+7];
    __xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__tmp_vec.push_back(_xlx_tmp_sc);
  }
  int __xlx_size_param_length = 1;
  int __xlx_offset_param_length = 1;
  int __xlx_offset_byte_param_length = 1*64;
  for (int j = 0, e = 1; j != e; ++j) {
    sc_bv<512> _xlx_tmp_sc;
    _xlx_tmp_sc.range(63, 0) = ((long long*)__xlx_apatb_param_is_dup)[j*8+0];
    _xlx_tmp_sc.range(127, 64) = ((long long*)__xlx_apatb_param_is_dup)[j*8+1];
    _xlx_tmp_sc.range(191, 128) = ((long long*)__xlx_apatb_param_is_dup)[j*8+2];
    _xlx_tmp_sc.range(255, 192) = ((long long*)__xlx_apatb_param_is_dup)[j*8+3];
    _xlx_tmp_sc.range(319, 256) = ((long long*)__xlx_apatb_param_is_dup)[j*8+4];
    _xlx_tmp_sc.range(383, 320) = ((long long*)__xlx_apatb_param_is_dup)[j*8+5];
    _xlx_tmp_sc.range(447, 384) = ((long long*)__xlx_apatb_param_is_dup)[j*8+6];
    _xlx_tmp_sc.range(511, 448) = ((long long*)__xlx_apatb_param_is_dup)[j*8+7];
    __xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__tmp_vec.push_back(_xlx_tmp_sc);
  }
  int __xlx_size_param_is_dup = 1;
  int __xlx_offset_param_is_dup = 2;
  int __xlx_offset_byte_param_is_dup = 2*64;
  for (int j = 0, e = 1; j != e; ++j) {
    sc_bv<512> _xlx_tmp_sc;
    _xlx_tmp_sc.range(63, 0) = ((long long*)__xlx_apatb_param_dup_index)[j*8+0];
    _xlx_tmp_sc.range(127, 64) = ((long long*)__xlx_apatb_param_dup_index)[j*8+1];
    _xlx_tmp_sc.range(191, 128) = ((long long*)__xlx_apatb_param_dup_index)[j*8+2];
    _xlx_tmp_sc.range(255, 192) = ((long long*)__xlx_apatb_param_dup_index)[j*8+3];
    _xlx_tmp_sc.range(319, 256) = ((long long*)__xlx_apatb_param_dup_index)[j*8+4];
    _xlx_tmp_sc.range(383, 320) = ((long long*)__xlx_apatb_param_dup_index)[j*8+5];
    _xlx_tmp_sc.range(447, 384) = ((long long*)__xlx_apatb_param_dup_index)[j*8+6];
    _xlx_tmp_sc.range(511, 448) = ((long long*)__xlx_apatb_param_dup_index)[j*8+7];
    __xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__tmp_vec.push_back(_xlx_tmp_sc);
  }
  int __xlx_size_param_dup_index = 1;
  int __xlx_offset_param_dup_index = 3;
  int __xlx_offset_byte_param_dup_index = 3*64;
  for (int j = 0, e = 1; j != e; ++j) {
    sc_bv<512> _xlx_tmp_sc;
    _xlx_tmp_sc.range(63, 0) = ((long long*)__xlx_apatb_param_temp_out_buffer)[j*8+0];
    _xlx_tmp_sc.range(127, 64) = ((long long*)__xlx_apatb_param_temp_out_buffer)[j*8+1];
    _xlx_tmp_sc.range(191, 128) = ((long long*)__xlx_apatb_param_temp_out_buffer)[j*8+2];
    _xlx_tmp_sc.range(255, 192) = ((long long*)__xlx_apatb_param_temp_out_buffer)[j*8+3];
    _xlx_tmp_sc.range(319, 256) = ((long long*)__xlx_apatb_param_temp_out_buffer)[j*8+4];
    _xlx_tmp_sc.range(383, 320) = ((long long*)__xlx_apatb_param_temp_out_buffer)[j*8+5];
    _xlx_tmp_sc.range(447, 384) = ((long long*)__xlx_apatb_param_temp_out_buffer)[j*8+6];
    _xlx_tmp_sc.range(511, 448) = ((long long*)__xlx_apatb_param_temp_out_buffer)[j*8+7];
    __xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__tmp_vec.push_back(_xlx_tmp_sc);
  }
  int __xlx_size_param_temp_out_buffer = 1;
  int __xlx_offset_param_temp_out_buffer = 4;
  int __xlx_offset_byte_param_temp_out_buffer = 4*64;
  for (int j = 0, e = 1; j != e; ++j) {
    sc_bv<512> _xlx_tmp_sc;
    _xlx_tmp_sc.range(63, 0) = ((long long*)__xlx_apatb_param_temp_out_buffer_size)[j*8+0];
    _xlx_tmp_sc.range(127, 64) = ((long long*)__xlx_apatb_param_temp_out_buffer_size)[j*8+1];
    _xlx_tmp_sc.range(191, 128) = ((long long*)__xlx_apatb_param_temp_out_buffer_size)[j*8+2];
    _xlx_tmp_sc.range(255, 192) = ((long long*)__xlx_apatb_param_temp_out_buffer_size)[j*8+3];
    _xlx_tmp_sc.range(319, 256) = ((long long*)__xlx_apatb_param_temp_out_buffer_size)[j*8+4];
    _xlx_tmp_sc.range(383, 320) = ((long long*)__xlx_apatb_param_temp_out_buffer_size)[j*8+5];
    _xlx_tmp_sc.range(447, 384) = ((long long*)__xlx_apatb_param_temp_out_buffer_size)[j*8+6];
    _xlx_tmp_sc.range(511, 448) = ((long long*)__xlx_apatb_param_temp_out_buffer_size)[j*8+7];
    __xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__tmp_vec.push_back(_xlx_tmp_sc);
  }
  int __xlx_size_param_temp_out_buffer_size = 1;
  int __xlx_offset_param_temp_out_buffer_size = 5;
  int __xlx_offset_byte_param_temp_out_buffer_size = 5*64;
  __cosim_s40__* __xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__input_buffer= new __cosim_s40__[__xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__tmp_vec.size()];
  for (int i = 0; i < __xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__tmp_vec.size(); ++i) {
    ((long long*)__xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__input_buffer)[i*8+0] = __xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__tmp_vec[i].range(63, 0).to_uint64();
    ((long long*)__xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__input_buffer)[i*8+1] = __xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__tmp_vec[i].range(127, 64).to_uint64();
    ((long long*)__xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__input_buffer)[i*8+2] = __xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__tmp_vec[i].range(191, 128).to_uint64();
    ((long long*)__xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__input_buffer)[i*8+3] = __xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__tmp_vec[i].range(255, 192).to_uint64();
    ((long long*)__xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__input_buffer)[i*8+4] = __xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__tmp_vec[i].range(319, 256).to_uint64();
    ((long long*)__xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__input_buffer)[i*8+5] = __xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__tmp_vec[i].range(383, 320).to_uint64();
    ((long long*)__xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__input_buffer)[i*8+6] = __xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__tmp_vec[i].range(447, 384).to_uint64();
    ((long long*)__xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__input_buffer)[i*8+7] = __xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__tmp_vec[i].range(511, 448).to_uint64();
  }
  // DUT call
  lzw_compress_hw(__xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__input_buffer, __xlx_offset_byte_param_s1, __xlx_offset_byte_param_length, __xlx_offset_byte_param_is_dup, __xlx_offset_byte_param_dup_index, __xlx_offset_byte_param_temp_out_buffer, __xlx_offset_byte_param_temp_out_buffer_size);
// print __xlx_apatb_param_s1
  sc_bv<512>*__xlx_s1_output_buffer = new sc_bv<512>[__xlx_size_param_s1];
  for (int i = 0; i < __xlx_size_param_s1; ++i) {
    char* start = (char*)(&(__xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__input_buffer[__xlx_offset_param_s1]));
    __xlx_s1_output_buffer[i].range(63, 0) = ((long long*)start)[i*8+0];
    __xlx_s1_output_buffer[i].range(127, 64) = ((long long*)start)[i*8+1];
    __xlx_s1_output_buffer[i].range(191, 128) = ((long long*)start)[i*8+2];
    __xlx_s1_output_buffer[i].range(255, 192) = ((long long*)start)[i*8+3];
    __xlx_s1_output_buffer[i].range(319, 256) = ((long long*)start)[i*8+4];
    __xlx_s1_output_buffer[i].range(383, 320) = ((long long*)start)[i*8+5];
    __xlx_s1_output_buffer[i].range(447, 384) = ((long long*)start)[i*8+6];
    __xlx_s1_output_buffer[i].range(511, 448) = ((long long*)start)[i*8+7];
  }
  for (int i = 0; i < __xlx_size_param_s1; ++i) {
    ((long long*)__xlx_apatb_param_s1)[i*8+0] = __xlx_s1_output_buffer[i].range(63, 0).to_uint64();
    ((long long*)__xlx_apatb_param_s1)[i*8+1] = __xlx_s1_output_buffer[i].range(127, 64).to_uint64();
    ((long long*)__xlx_apatb_param_s1)[i*8+2] = __xlx_s1_output_buffer[i].range(191, 128).to_uint64();
    ((long long*)__xlx_apatb_param_s1)[i*8+3] = __xlx_s1_output_buffer[i].range(255, 192).to_uint64();
    ((long long*)__xlx_apatb_param_s1)[i*8+4] = __xlx_s1_output_buffer[i].range(319, 256).to_uint64();
    ((long long*)__xlx_apatb_param_s1)[i*8+5] = __xlx_s1_output_buffer[i].range(383, 320).to_uint64();
    ((long long*)__xlx_apatb_param_s1)[i*8+6] = __xlx_s1_output_buffer[i].range(447, 384).to_uint64();
    ((long long*)__xlx_apatb_param_s1)[i*8+7] = __xlx_s1_output_buffer[i].range(511, 448).to_uint64();
  }
// print __xlx_apatb_param_length
  sc_bv<512>*__xlx_length_output_buffer = new sc_bv<512>[__xlx_size_param_length];
  for (int i = 0; i < __xlx_size_param_length; ++i) {
    char* start = (char*)(&(__xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__input_buffer[__xlx_offset_param_length]));
    __xlx_length_output_buffer[i].range(63, 0) = ((long long*)start)[i*8+0];
    __xlx_length_output_buffer[i].range(127, 64) = ((long long*)start)[i*8+1];
    __xlx_length_output_buffer[i].range(191, 128) = ((long long*)start)[i*8+2];
    __xlx_length_output_buffer[i].range(255, 192) = ((long long*)start)[i*8+3];
    __xlx_length_output_buffer[i].range(319, 256) = ((long long*)start)[i*8+4];
    __xlx_length_output_buffer[i].range(383, 320) = ((long long*)start)[i*8+5];
    __xlx_length_output_buffer[i].range(447, 384) = ((long long*)start)[i*8+6];
    __xlx_length_output_buffer[i].range(511, 448) = ((long long*)start)[i*8+7];
  }
  for (int i = 0; i < __xlx_size_param_length; ++i) {
    ((long long*)__xlx_apatb_param_length)[i*8+0] = __xlx_length_output_buffer[i].range(63, 0).to_uint64();
    ((long long*)__xlx_apatb_param_length)[i*8+1] = __xlx_length_output_buffer[i].range(127, 64).to_uint64();
    ((long long*)__xlx_apatb_param_length)[i*8+2] = __xlx_length_output_buffer[i].range(191, 128).to_uint64();
    ((long long*)__xlx_apatb_param_length)[i*8+3] = __xlx_length_output_buffer[i].range(255, 192).to_uint64();
    ((long long*)__xlx_apatb_param_length)[i*8+4] = __xlx_length_output_buffer[i].range(319, 256).to_uint64();
    ((long long*)__xlx_apatb_param_length)[i*8+5] = __xlx_length_output_buffer[i].range(383, 320).to_uint64();
    ((long long*)__xlx_apatb_param_length)[i*8+6] = __xlx_length_output_buffer[i].range(447, 384).to_uint64();
    ((long long*)__xlx_apatb_param_length)[i*8+7] = __xlx_length_output_buffer[i].range(511, 448).to_uint64();
  }
// print __xlx_apatb_param_is_dup
  sc_bv<512>*__xlx_is_dup_output_buffer = new sc_bv<512>[__xlx_size_param_is_dup];
  for (int i = 0; i < __xlx_size_param_is_dup; ++i) {
    char* start = (char*)(&(__xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__input_buffer[__xlx_offset_param_is_dup]));
    __xlx_is_dup_output_buffer[i].range(63, 0) = ((long long*)start)[i*8+0];
    __xlx_is_dup_output_buffer[i].range(127, 64) = ((long long*)start)[i*8+1];
    __xlx_is_dup_output_buffer[i].range(191, 128) = ((long long*)start)[i*8+2];
    __xlx_is_dup_output_buffer[i].range(255, 192) = ((long long*)start)[i*8+3];
    __xlx_is_dup_output_buffer[i].range(319, 256) = ((long long*)start)[i*8+4];
    __xlx_is_dup_output_buffer[i].range(383, 320) = ((long long*)start)[i*8+5];
    __xlx_is_dup_output_buffer[i].range(447, 384) = ((long long*)start)[i*8+6];
    __xlx_is_dup_output_buffer[i].range(511, 448) = ((long long*)start)[i*8+7];
  }
  for (int i = 0; i < __xlx_size_param_is_dup; ++i) {
    ((long long*)__xlx_apatb_param_is_dup)[i*8+0] = __xlx_is_dup_output_buffer[i].range(63, 0).to_uint64();
    ((long long*)__xlx_apatb_param_is_dup)[i*8+1] = __xlx_is_dup_output_buffer[i].range(127, 64).to_uint64();
    ((long long*)__xlx_apatb_param_is_dup)[i*8+2] = __xlx_is_dup_output_buffer[i].range(191, 128).to_uint64();
    ((long long*)__xlx_apatb_param_is_dup)[i*8+3] = __xlx_is_dup_output_buffer[i].range(255, 192).to_uint64();
    ((long long*)__xlx_apatb_param_is_dup)[i*8+4] = __xlx_is_dup_output_buffer[i].range(319, 256).to_uint64();
    ((long long*)__xlx_apatb_param_is_dup)[i*8+5] = __xlx_is_dup_output_buffer[i].range(383, 320).to_uint64();
    ((long long*)__xlx_apatb_param_is_dup)[i*8+6] = __xlx_is_dup_output_buffer[i].range(447, 384).to_uint64();
    ((long long*)__xlx_apatb_param_is_dup)[i*8+7] = __xlx_is_dup_output_buffer[i].range(511, 448).to_uint64();
  }
// print __xlx_apatb_param_dup_index
  sc_bv<512>*__xlx_dup_index_output_buffer = new sc_bv<512>[__xlx_size_param_dup_index];
  for (int i = 0; i < __xlx_size_param_dup_index; ++i) {
    char* start = (char*)(&(__xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__input_buffer[__xlx_offset_param_dup_index]));
    __xlx_dup_index_output_buffer[i].range(63, 0) = ((long long*)start)[i*8+0];
    __xlx_dup_index_output_buffer[i].range(127, 64) = ((long long*)start)[i*8+1];
    __xlx_dup_index_output_buffer[i].range(191, 128) = ((long long*)start)[i*8+2];
    __xlx_dup_index_output_buffer[i].range(255, 192) = ((long long*)start)[i*8+3];
    __xlx_dup_index_output_buffer[i].range(319, 256) = ((long long*)start)[i*8+4];
    __xlx_dup_index_output_buffer[i].range(383, 320) = ((long long*)start)[i*8+5];
    __xlx_dup_index_output_buffer[i].range(447, 384) = ((long long*)start)[i*8+6];
    __xlx_dup_index_output_buffer[i].range(511, 448) = ((long long*)start)[i*8+7];
  }
  for (int i = 0; i < __xlx_size_param_dup_index; ++i) {
    ((long long*)__xlx_apatb_param_dup_index)[i*8+0] = __xlx_dup_index_output_buffer[i].range(63, 0).to_uint64();
    ((long long*)__xlx_apatb_param_dup_index)[i*8+1] = __xlx_dup_index_output_buffer[i].range(127, 64).to_uint64();
    ((long long*)__xlx_apatb_param_dup_index)[i*8+2] = __xlx_dup_index_output_buffer[i].range(191, 128).to_uint64();
    ((long long*)__xlx_apatb_param_dup_index)[i*8+3] = __xlx_dup_index_output_buffer[i].range(255, 192).to_uint64();
    ((long long*)__xlx_apatb_param_dup_index)[i*8+4] = __xlx_dup_index_output_buffer[i].range(319, 256).to_uint64();
    ((long long*)__xlx_apatb_param_dup_index)[i*8+5] = __xlx_dup_index_output_buffer[i].range(383, 320).to_uint64();
    ((long long*)__xlx_apatb_param_dup_index)[i*8+6] = __xlx_dup_index_output_buffer[i].range(447, 384).to_uint64();
    ((long long*)__xlx_apatb_param_dup_index)[i*8+7] = __xlx_dup_index_output_buffer[i].range(511, 448).to_uint64();
  }
// print __xlx_apatb_param_temp_out_buffer
  sc_bv<512>*__xlx_temp_out_buffer_output_buffer = new sc_bv<512>[__xlx_size_param_temp_out_buffer];
  for (int i = 0; i < __xlx_size_param_temp_out_buffer; ++i) {
    char* start = (char*)(&(__xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__input_buffer[__xlx_offset_param_temp_out_buffer]));
    __xlx_temp_out_buffer_output_buffer[i].range(63, 0) = ((long long*)start)[i*8+0];
    __xlx_temp_out_buffer_output_buffer[i].range(127, 64) = ((long long*)start)[i*8+1];
    __xlx_temp_out_buffer_output_buffer[i].range(191, 128) = ((long long*)start)[i*8+2];
    __xlx_temp_out_buffer_output_buffer[i].range(255, 192) = ((long long*)start)[i*8+3];
    __xlx_temp_out_buffer_output_buffer[i].range(319, 256) = ((long long*)start)[i*8+4];
    __xlx_temp_out_buffer_output_buffer[i].range(383, 320) = ((long long*)start)[i*8+5];
    __xlx_temp_out_buffer_output_buffer[i].range(447, 384) = ((long long*)start)[i*8+6];
    __xlx_temp_out_buffer_output_buffer[i].range(511, 448) = ((long long*)start)[i*8+7];
  }
  for (int i = 0; i < __xlx_size_param_temp_out_buffer; ++i) {
    ((long long*)__xlx_apatb_param_temp_out_buffer)[i*8+0] = __xlx_temp_out_buffer_output_buffer[i].range(63, 0).to_uint64();
    ((long long*)__xlx_apatb_param_temp_out_buffer)[i*8+1] = __xlx_temp_out_buffer_output_buffer[i].range(127, 64).to_uint64();
    ((long long*)__xlx_apatb_param_temp_out_buffer)[i*8+2] = __xlx_temp_out_buffer_output_buffer[i].range(191, 128).to_uint64();
    ((long long*)__xlx_apatb_param_temp_out_buffer)[i*8+3] = __xlx_temp_out_buffer_output_buffer[i].range(255, 192).to_uint64();
    ((long long*)__xlx_apatb_param_temp_out_buffer)[i*8+4] = __xlx_temp_out_buffer_output_buffer[i].range(319, 256).to_uint64();
    ((long long*)__xlx_apatb_param_temp_out_buffer)[i*8+5] = __xlx_temp_out_buffer_output_buffer[i].range(383, 320).to_uint64();
    ((long long*)__xlx_apatb_param_temp_out_buffer)[i*8+6] = __xlx_temp_out_buffer_output_buffer[i].range(447, 384).to_uint64();
    ((long long*)__xlx_apatb_param_temp_out_buffer)[i*8+7] = __xlx_temp_out_buffer_output_buffer[i].range(511, 448).to_uint64();
  }
// print __xlx_apatb_param_temp_out_buffer_size
  sc_bv<512>*__xlx_temp_out_buffer_size_output_buffer = new sc_bv<512>[__xlx_size_param_temp_out_buffer_size];
  for (int i = 0; i < __xlx_size_param_temp_out_buffer_size; ++i) {
    char* start = (char*)(&(__xlx_s1_length_is_dup_dup_index_temp_out_buffer_temp_out_buffer_size__input_buffer[__xlx_offset_param_temp_out_buffer_size]));
    __xlx_temp_out_buffer_size_output_buffer[i].range(63, 0) = ((long long*)start)[i*8+0];
    __xlx_temp_out_buffer_size_output_buffer[i].range(127, 64) = ((long long*)start)[i*8+1];
    __xlx_temp_out_buffer_size_output_buffer[i].range(191, 128) = ((long long*)start)[i*8+2];
    __xlx_temp_out_buffer_size_output_buffer[i].range(255, 192) = ((long long*)start)[i*8+3];
    __xlx_temp_out_buffer_size_output_buffer[i].range(319, 256) = ((long long*)start)[i*8+4];
    __xlx_temp_out_buffer_size_output_buffer[i].range(383, 320) = ((long long*)start)[i*8+5];
    __xlx_temp_out_buffer_size_output_buffer[i].range(447, 384) = ((long long*)start)[i*8+6];
    __xlx_temp_out_buffer_size_output_buffer[i].range(511, 448) = ((long long*)start)[i*8+7];
  }
  for (int i = 0; i < __xlx_size_param_temp_out_buffer_size; ++i) {
    ((long long*)__xlx_apatb_param_temp_out_buffer_size)[i*8+0] = __xlx_temp_out_buffer_size_output_buffer[i].range(63, 0).to_uint64();
    ((long long*)__xlx_apatb_param_temp_out_buffer_size)[i*8+1] = __xlx_temp_out_buffer_size_output_buffer[i].range(127, 64).to_uint64();
    ((long long*)__xlx_apatb_param_temp_out_buffer_size)[i*8+2] = __xlx_temp_out_buffer_size_output_buffer[i].range(191, 128).to_uint64();
    ((long long*)__xlx_apatb_param_temp_out_buffer_size)[i*8+3] = __xlx_temp_out_buffer_size_output_buffer[i].range(255, 192).to_uint64();
    ((long long*)__xlx_apatb_param_temp_out_buffer_size)[i*8+4] = __xlx_temp_out_buffer_size_output_buffer[i].range(319, 256).to_uint64();
    ((long long*)__xlx_apatb_param_temp_out_buffer_size)[i*8+5] = __xlx_temp_out_buffer_size_output_buffer[i].range(383, 320).to_uint64();
    ((long long*)__xlx_apatb_param_temp_out_buffer_size)[i*8+6] = __xlx_temp_out_buffer_size_output_buffer[i].range(447, 384).to_uint64();
    ((long long*)__xlx_apatb_param_temp_out_buffer_size)[i*8+7] = __xlx_temp_out_buffer_size_output_buffer[i].range(511, 448).to_uint64();
  }
}
