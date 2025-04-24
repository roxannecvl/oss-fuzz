# Script with seed
cd oss-fuzz
git checkout main
rm -rf build/out/libjpeg-turbo/*
python3 infra/helper.py build_fuzzers libjpeg-turbo
mkdir -p build/out/corpus_seed/
# Store results to build/out/corpus_seed
python3 infra/helper.py run_fuzzer libjpeg-turbo decompress_yuv_fuzzer --corpus-dir build/out/corpus_seed
