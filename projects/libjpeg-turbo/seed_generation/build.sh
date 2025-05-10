#gcc -o generate_seeds generate_seeds.c -lturbojpeg
# Building the harness
clang++ -g -fsanitize=fuzzer,address,undefined save_image.cc -o save_image_fuzzer -lturbojpeg
# Debugging with one seed
gdb -args ./save_image_fuzzer seed_generation/out_copy/0b237a4bd9e19f42209f3c349d38b2a6.seed
