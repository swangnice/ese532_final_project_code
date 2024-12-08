; ModuleID = '/mnt/castor/seas_home/s/swang01/Documents/ese532_code/ese532_final_project_code/lzw_co/solution1/.autopilot/db/a.g.ld.5.gdce.bc'
source_filename = "llvm-link"
target datalayout = "e-m:e-i64:64-i128:128-i256:256-i512:512-i1024:1024-i2048:2048-i4096:4096-n8:16:32:64-S128-v16:16-v24:32-v32:32-v48:64-v96:128-v192:256-v256:256-v512:512-v1024:1024"
target triple = "fpga64-xilinx-none"

; Function Attrs: argmemonly noinline
define void @apatb_lzw_compress_hw_ir(i8* %s1, i32* %length, i32* %is_dup, i32* %dup_index, i8* %temp_out_buffer, i32* %temp_out_buffer_size) local_unnamed_addr #0 {
entry:
  %s1_copy = alloca [2048 x i8], align 512
  %length_copy = alloca i32, align 512
  %is_dup_copy = alloca i32, align 512
  %dup_index_copy = alloca i32, align 512
  %temp_out_buffer_copy = alloca [2048 x i8], align 512
  %temp_out_buffer_size_copy = alloca i32, align 512
  %0 = bitcast i8* %s1 to [2048 x i8]*
  %1 = bitcast i8* %temp_out_buffer to [2048 x i8]*
  call fastcc void @copy_in([2048 x i8]* %0, [2048 x i8]* nonnull align 512 %s1_copy, i32* %length, i32* nonnull align 512 %length_copy, i32* %is_dup, i32* nonnull align 512 %is_dup_copy, i32* %dup_index, i32* nonnull align 512 %dup_index_copy, [2048 x i8]* %1, [2048 x i8]* nonnull align 512 %temp_out_buffer_copy, i32* %temp_out_buffer_size, i32* nonnull align 512 %temp_out_buffer_size_copy)
  %2 = getelementptr inbounds [2048 x i8], [2048 x i8]* %s1_copy, i32 0, i32 0
  %3 = getelementptr inbounds [2048 x i8], [2048 x i8]* %temp_out_buffer_copy, i32 0, i32 0
  call void @apatb_lzw_compress_hw_hw(i8* %2, i32* %length_copy, i32* %is_dup_copy, i32* %dup_index_copy, i8* %3, i32* %temp_out_buffer_size_copy)
  call fastcc void @copy_out([2048 x i8]* %0, [2048 x i8]* nonnull align 512 %s1_copy, i32* %length, i32* nonnull align 512 %length_copy, i32* %is_dup, i32* nonnull align 512 %is_dup_copy, i32* %dup_index, i32* nonnull align 512 %dup_index_copy, [2048 x i8]* %1, [2048 x i8]* nonnull align 512 %temp_out_buffer_copy, i32* %temp_out_buffer_size, i32* nonnull align 512 %temp_out_buffer_size_copy)
  ret void
}

; Function Attrs: argmemonly noinline
define internal fastcc void @copy_in([2048 x i8]* readonly, [2048 x i8]* noalias align 512, i32* readonly, i32* noalias align 512, i32* readonly, i32* noalias align 512, i32* readonly, i32* noalias align 512, [2048 x i8]* readonly, [2048 x i8]* noalias align 512, i32* readonly, i32* noalias align 512) unnamed_addr #1 {
entry:
  call fastcc void @onebyonecpy_hls.p0a2048i8([2048 x i8]* align 512 %1, [2048 x i8]* %0)
  call fastcc void @onebyonecpy_hls.p0i32(i32* align 512 %3, i32* %2)
  call fastcc void @onebyonecpy_hls.p0i32(i32* align 512 %5, i32* %4)
  call fastcc void @onebyonecpy_hls.p0i32(i32* align 512 %7, i32* %6)
  call fastcc void @onebyonecpy_hls.p0a2048i8([2048 x i8]* align 512 %9, [2048 x i8]* %8)
  call fastcc void @onebyonecpy_hls.p0i32(i32* align 512 %11, i32* %10)
  ret void
}

; Function Attrs: argmemonly noinline
define internal fastcc void @onebyonecpy_hls.p0a2048i8([2048 x i8]* noalias align 512, [2048 x i8]* noalias readonly) unnamed_addr #2 {
entry:
  %2 = icmp eq [2048 x i8]* %0, null
  %3 = icmp eq [2048 x i8]* %1, null
  %4 = or i1 %2, %3
  br i1 %4, label %ret, label %copy

copy:                                             ; preds = %entry
  br label %for.loop

for.loop:                                         ; preds = %for.loop, %copy
  %for.loop.idx1 = phi i64 [ 0, %copy ], [ %for.loop.idx.next, %for.loop ]
  %dst.addr = getelementptr [2048 x i8], [2048 x i8]* %0, i64 0, i64 %for.loop.idx1
  %src.addr = getelementptr [2048 x i8], [2048 x i8]* %1, i64 0, i64 %for.loop.idx1
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 1 %dst.addr, i8* align 1 %src.addr, i64 1, i1 false)
  %for.loop.idx.next = add nuw nsw i64 %for.loop.idx1, 1
  %exitcond = icmp ne i64 %for.loop.idx.next, 2048
  br i1 %exitcond, label %for.loop, label %ret

ret:                                              ; preds = %for.loop, %entry
  ret void
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i1) #3

; Function Attrs: argmemonly noinline
define internal fastcc void @onebyonecpy_hls.p0i32(i32* noalias align 512, i32* noalias readonly) unnamed_addr #2 {
entry:
  %2 = icmp eq i32* %0, null
  %3 = icmp eq i32* %1, null
  %4 = or i1 %2, %3
  br i1 %4, label %ret, label %copy

copy:                                             ; preds = %entry
  %5 = bitcast i32* %0 to i8*
  %6 = bitcast i32* %1 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 1 %5, i8* align 1 %6, i64 4, i1 false)
  br label %ret

ret:                                              ; preds = %copy, %entry
  ret void
}

; Function Attrs: argmemonly noinline
define internal fastcc void @copy_out([2048 x i8]*, [2048 x i8]* noalias readonly align 512, i32*, i32* noalias readonly align 512, i32*, i32* noalias readonly align 512, i32*, i32* noalias readonly align 512, [2048 x i8]*, [2048 x i8]* noalias readonly align 512, i32*, i32* noalias readonly align 512) unnamed_addr #4 {
entry:
  call fastcc void @onebyonecpy_hls.p0a2048i8([2048 x i8]* %0, [2048 x i8]* align 512 %1)
  call fastcc void @onebyonecpy_hls.p0i32(i32* %2, i32* align 512 %3)
  call fastcc void @onebyonecpy_hls.p0i32(i32* %4, i32* align 512 %5)
  call fastcc void @onebyonecpy_hls.p0i32(i32* %6, i32* align 512 %7)
  call fastcc void @onebyonecpy_hls.p0a2048i8([2048 x i8]* %8, [2048 x i8]* align 512 %9)
  call fastcc void @onebyonecpy_hls.p0i32(i32* %10, i32* align 512 %11)
  ret void
}

declare void @apatb_lzw_compress_hw_hw(i8*, i32*, i32*, i32*, i8*, i32*)

define void @lzw_compress_hw_hw_stub_wrapper(i8*, i32*, i32*, i32*, i8*, i32*) #5 {
entry:
  %6 = bitcast i8* %0 to [2048 x i8]*
  %7 = bitcast i8* %4 to [2048 x i8]*
  call void @copy_out([2048 x i8]* null, [2048 x i8]* %6, i32* null, i32* %1, i32* null, i32* %2, i32* null, i32* %3, [2048 x i8]* null, [2048 x i8]* %7, i32* null, i32* %5)
  %8 = bitcast [2048 x i8]* %6 to i8*
  %9 = bitcast [2048 x i8]* %7 to i8*
  call void @lzw_compress_hw_hw_stub(i8* %8, i32* %1, i32* %2, i32* %3, i8* %9, i32* %5)
  call void @copy_in([2048 x i8]* null, [2048 x i8]* %6, i32* null, i32* %1, i32* null, i32* %2, i32* null, i32* %3, [2048 x i8]* null, [2048 x i8]* %7, i32* null, i32* %5)
  ret void
}

declare void @lzw_compress_hw_hw_stub(i8*, i32*, i32*, i32*, i8*, i32*)

attributes #0 = { argmemonly noinline "fpga.wrapper.func"="wrapper" }
attributes #1 = { argmemonly noinline "fpga.wrapper.func"="copyin" }
attributes #2 = { argmemonly noinline "fpga.wrapper.func"="onebyonecpy_hls" }
attributes #3 = { argmemonly nounwind }
attributes #4 = { argmemonly noinline "fpga.wrapper.func"="copyout" }
attributes #5 = { "fpga.wrapper.func"="stub" }

!llvm.dbg.cu = !{}
!llvm.ident = !{!0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0}
!llvm.module.flags = !{!1, !2, !3}
!blackbox_cfg = !{!4}

!0 = !{!"clang version 7.0.0 "}
!1 = !{i32 2, !"Dwarf Version", i32 4}
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = !{i32 1, !"wchar_size", i32 4}
!4 = !{}
