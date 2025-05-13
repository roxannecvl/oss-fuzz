git checkout main
python3 infra/helper.py build_image libjpeg-turbo
# Clean previous builds
rm -rf build/out/libjpeg-turbo/*

# Build fuzzers
python3 infra/helper.py build_fuzzers libjpeg-turbo

for i in {1..3}; do
    corpus_dir="build/out/corpus_seed$i"

    # Clear corpus dir if it exists
    if [ -d "$corpus_dir" ]; then
        echo "Clearing existing contents of $corpus_dir"
        rm -rf "$corpus_dir"/*
    fi

    mkdir -p "$corpus_dir"
    echo "Running fuzz iteration $i for 10s..."
    python3 infra/helper.py run_fuzzer libjpeg-turbo libjpeg_turbo_fuzzer \
        --corpus-dir "$corpus_dir"
done

# Merge corpora into one
merged_corpus="build/out/full_corpus_seed"
mkdir -p "$merged_corpus"
echo "Merging all corpus directories into $merged_corpus..."
cp -a build/out/corpus_seed1/. "$merged_corpus/"
cp -a build/out/corpus_seed2/. "$merged_corpus/"
cp -a build/out/corpus_seed3/. "$merged_corpus/"

# Build for coverage
echo "Building with coverage sanitizer..."
python3 infra/helper.py build_fuzzers --sanitizer coverage libjpeg-turbo

# Run coverage
echo "Running coverage analysis..."
python3 infra/helper.py coverage libjpeg-turbo \
    --corpus-dir "$merged_corpus" \
    --fuzz-target libjpeg_turbo_fuzzer

