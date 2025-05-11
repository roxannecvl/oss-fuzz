import os
import subprocess

INPUT_DIR = "jpeg"
OUTPUT_DIR = "out"
GENERATOR_BIN = "./generate_seed"  # Update if your binary has a different name

os.makedirs(OUTPUT_DIR, exist_ok=True)

for fname in os.listdir(INPUT_DIR):
    if not fname.lower().endswith((".jpg", ".jpeg")):
        continue

    input_path = os.path.join(INPUT_DIR, fname)
    seed_name = os.path.splitext(fname)[0] + ".seed"
    output_path = os.path.join(OUTPUT_DIR, seed_name)

    try:
        subprocess.run(
            [GENERATOR_BIN, input_path, output_path],
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        print(f"✅ Generated seed: {output_path}")
    except subprocess.CalledProcessError as e:
        print(f"❌ Failed to generate seed for {fname}: {e.stderr.decode().strip()}")
