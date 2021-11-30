gcc -c -g -O0 -fPIC instrument_static_lib.c  -o instrument_static_lib.o
ar rcs libinstrument_static_lib.a instrument_static_lib.o
cd blankit;
gcc -c -g -O0 -fPIC blankit.c -o blankit.o
ar rcs libblankit_static_lib.a blankit.o
