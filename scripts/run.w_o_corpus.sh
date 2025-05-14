git clone git@github.com:roxannecvl/oss-fuzz.git && cd oss-fuzz
python3 infra/helper.py build_image libjpeg-turbo
git checkout no-seed

rm -rf build/out/libjpeg-turbo/*
python3 infra/helper.py build_fuzzers libjpeg-turbo
mkdir -p build/out/corpus_no_seed/
# Store results to build/out/corpus_no_seed
python3 infra/helper.py run_fuzzer libjpeg-turbo libjpeg_turbo_fuzzer --corpus-dir build/out/corpus_no_seed

# Get coverage
python3 infra/helper.py build_fuzzers --sanitizer coverage libjpeg-turbo
python3 infra/helper.py coverage libjpeg-turbo --corpus-dir build/out/corpus_no_seed/ --fuzz-target libjpeg_turbo_fuzzer
