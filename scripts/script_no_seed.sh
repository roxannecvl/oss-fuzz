# Script without seed
cd oss-fuzz
git checkout no_seed
rm -rf build/out/libjpeg-turbo/*
python3 infra/helper.py build_fuzzers libjpeg-turbo
mkdir -p build/out/corpus_no_seed/
# Store results to build/out/corpus_no_seed
python3 infra/helper.py run_fuzzer libjpeg-turbo compress_fuzzer --corpus-dir build/out/corpus_no_seed