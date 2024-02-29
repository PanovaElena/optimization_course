@echo off

set list_number_run=0 1 2

REM insert

set size_list=256 512 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288
set size_advi=131072
set program_list=insert_v0 insert_v1 insert_v2 insert_v3 insert_v4

(for %%P in (%program_list%) do (
	icl /debug /O2 /Qopt-report=5 /QxHost %%P\%%P.cpp -o %%P\%%P
))

(for %%P in (%program_list%) do (
	echo %%P
    (for %%s in (%size_list%) do (
        (for %%i in (%list_number_run%) do (
            %%P\%%P %%s
        ))
    ))
))

(for %%P in (%program_list%) do (  
    echo advisor %%P
    advisor --collect=roofline --project-dir=%%P\advi_%%P -- %%P\%%P %size_advi%
))

REM quick

set size_list=256 512 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608 16777216 33554432 67108864
set size_advi=8388608
set program_list=quick_v0 quick_v1 quick_v2 quick_v3 quick_v4 quick_v5 quick_v6

(for %%P in (%program_list%) do (
	icl /debug /O2 /Qopt-report=5 /QxHost %%P\%%P.cpp -o %%P\%%P
))

(for %%P in (%program_list%) do (
	echo %%P
    (for %%s in (%size_list%) do (
        (for %%i in (%list_number_run%) do (
            %%P\%%P %%s
        ))
    ))
))

(for %%P in (%program_list%) do (  
    echo advisor %%P
    advisor --collect=roofline --project-dir=%%P\advi_%%P -- %%P\%%P %size_advi%
))

echo done
