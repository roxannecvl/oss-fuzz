import subprocess
import os
import sys

def is_high_precision(filepath):
    try:
        result = subprocess.run(
            ['exiftool', '-BitsPerSample', filepath],
            capture_output=True,
            text=True,
            check=True
        )
        for line in result.stdout.splitlines():
            if 'Bits Per Sample' in line:
                bit_depth = int(line.split(':')[-1].strip())
                if bit_depth in (12, 16):
                    return True
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
    return False

def scan_directory(folder):
    for root, _, files in os.walk(folder):
        for f in files:
            if f.lower().endswith(('.jpg', '.jpeg')):
                path = os.path.join(root, f)
                if is_high_precision(path):
                    print(f"High precision: {path}")

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python check_precision.py <directory>")
        sys.exit(1)
    scan_directory(sys.argv[1])
