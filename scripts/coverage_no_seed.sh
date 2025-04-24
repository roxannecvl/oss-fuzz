cd oss-fuzz
python3 infra/helper.py build_fuzzers --sanitizer coverage libjpeg-turbo
python3 infra/helper.py coverage libjpeg-turbo --corpus-dir build/out/corpus_no_seed/ --fuzz-target decompress_yuv_fuzzer
