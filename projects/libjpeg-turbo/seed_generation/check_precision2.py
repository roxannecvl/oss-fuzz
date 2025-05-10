import os
import sys
from pathlib import Path

def get_jpeg_precision(filepath):
    try:
        with open(filepath, 'rb') as f:
            data = f.read()
            i = 0
            while i < len(data) - 1:
                if data[i] == 0xFF:
                    marker = data[i + 1]
                    # SOF0 to SOF15 (excluding DHT, JPG, DAC)
                    if 0xC0 <= marker <= 0xCF and marker not in (0xC4, 0xC8, 0xCC):
                        # Precision is at offset i+4
                        return data[i + 4]
                    i += 2
                else:
                    i += 1
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
    return None

def scan_directory(folder):
    for file in os.listdir(folder):
        filename = os.fsdecode(file)
        if filename.endswith(".JPEG") or filename.endswith(".jpeg") or filename.endswith(".jpg"):
            if get_jpeg_precision(os.path.join(folder, filename)) in (12, 16):
                print(filename)
            continue
        else:
            continue

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python check_precision_pure.py <directory>")
        sys.exit(1)
    scan_directory(sys.argv[1])
