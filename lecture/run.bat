@echo off

set list_number_run=0 1 2 3 4

set program_list_compile_simple=gamma_rgb_v8_zmm_novec gamma_rgb_v0_base_novec gamma_rgb_v1_ivdep integral_v0_novec reduction_v0_novec
set program_list_compile_vec=gamma_rgb_v2_xHost gamma_rgb_v3_unroll gamma_rgb_v4_mem_access gamma_rgb_v5_type gamma_rgb_v6_mem_align gamma_rgba_v0_novec
set program_list_compile_zmm=gamma_rgb_v7_zmm gamma_rgba_v1_vec integral_v1_try_vec integral_v2_pragma_simd reduction_v1_vec
set program_list=%program_list_compile_simple% %program_list_compile_vec% %program_list_compile_zmm%


(for %%P in (%program_list_compile_simple%) do (
	icl /debug /O2 /Qopt-report=5 -I%%P\bmp_reader.h %%P\%%P.cpp -o %%P\%%P
))
(for %%P in (%program_list_compile_vec%) do (
    icl /debug /O2 /Qopt-report=5 /QxHost -I%%P\bmp_reader.h %%P\%%P.cpp -o %%P\%%P
))
(for %%P in (%program_list_compile_zmm%) do (
	icl /debug /O2 /Qopt-report=5 /QxHost /Qopt-zmm-usage=high -I%%P\bmp_reader.h %%P\%%P.cpp -o %%P\%%P
))


(for %%P in (%program_list%) do (
	echo %%P	
    (for %%i in (%list_number_run%) do (
        %%P\%%P
    ))
))
