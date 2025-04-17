# First cript for setup
# Ensure we are cloning our fork of oss-fuzz to ensure /infra/helper.py is updated 
# with 'timeout', '4h' at line 1445
git clone git@github.com:roxannecvl/oss-fuzz.git && cd oss-fuzz
python3 infra/helper.py build_image libjpeg-turbo