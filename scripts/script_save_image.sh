rm -rf build/out/libjpeg-turbo/*
python3 infra/helper.py build_fuzzers libjpeg-turbo
mkdir -p build/out/corpus_seed/
python3 infra/helper.py run_fuzzer libjpeg-turbo save_image_fuzzer --corpus-dir build/out/corpus_seed