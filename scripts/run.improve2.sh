git clone git@github.com:roxannecvl/oss-fuzz.git && cd oss-fuzz
# Suposed to be ran from the oss-fuzz root directory
python3 infra/helper.py build_image libjpeg-turbo
git checkout tj3SaveImage
rm -rf build/out/libjpeg-turbo/*
# Download the new seed corpus
mkdir projects/libjpeg-turbo/seed_generation
wget -O projects/libjpeg-turbo/seed_generation/save_image_corpus.zip  https://paultissot.ch/save_image_corpus.zip
python3 infra/helper.py build_fuzzers libjpeg-turbo
mkdir -p build/out/corpus_seed/
# Store results to build/out/corpus_seed
python3 infra/helper.py run_fuzzer libjpeg-turbo save_image_fuzzer --corpus-dir build/out/corpus_seed

# Get coverage
python3 infra/helper.py build_fuzzers --sanitizer coverage libjpeg-turbo
python3 infra/helper.py coverage libjpeg-turbo --corpus-dir build/out/corpus_seed/ --fuzz-target save_image_fuzzer
