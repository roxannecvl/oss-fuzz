#!/usr/bin/env python3
import os
import sys
import shutil
import argparse
from bs4 import BeautifulSoup

def parse_number(text):
    text = text.strip()
    if text.endswith('k'):
        return int(float(text[:-1]) * 1_000)
    elif text.endswith('M'):
        return int(float(text[:-1]) * 1_000_000)
    elif text.endswith('E'):
        return int(float(text[:-1]) * 1_000_000)
    elif text.endswith('G'):
        return int(float(text[:-1]) * 1_000_000_000)
    else:
        return int(text)

def format_number(n):
    if n >= 1_000_000:
        return f"{n / 1_000_000:.2f}M".rstrip('0').rstrip('.')
    elif n >= 1_000:
        return f"{n / 1_000:.2f}k".rstrip('0').rstrip('.')
    else:
        return str(n)


def extract_count(td):
    return parse_number(td.find('pre').get_text(strip=True))

def merge_reports(dir1, dir2, dir3, out_dir):
    # 1) copy entire tree from dir1 into out_dir
    if os.path.exists(out_dir):
        print(f"Error: output directory '{out_dir}' already exists.", file=sys.stderr)
        sys.exit(1)
    shutil.copytree(dir1, out_dir)

    # 2) walk through every .html in dir1
    for root, dirs, files in os.walk(dir1):
        rel = os.path.relpath(root, dir1)
        for fn in files:
            if not fn.endswith(".html"):
                continue
            # skip the top-level indexes/reports
            if fn in ("index.html", "report.html",
                      "file_view_index.html", "directory_view_index.html"):
                continue

            path1 = os.path.join(dir1,  rel, fn)
            path2 = os.path.join(dir2,  rel, fn)
            path3 = os.path.join(dir3,  rel, fn)

            if not (os.path.exists(path2) and os.path.exists(path3)):
                # if any report is missing this file, just leave the template copy untouched
                continue

            # parse all three
            with open(path1, encoding="utf8") as f1, \
                 open(path2, encoding="utf8") as f2, \
                 open(path3, encoding="utf8") as f3:
                soup1 = BeautifulSoup(f1, "html.parser")
                soup2 = BeautifulSoup(f2, "html.parser")
                soup3 = BeautifulSoup(f3, "html.parser")

            # find the per-line execution‐count cells; adjust the class name if yours differs
            cols1 = soup1.find_all("td", class_="covered-line")
            cols2 = soup2.find_all("td", class_="covered-line")
            cols3 = soup3.find_all("td", class_="covered-line")

            # sanity check
            if not (len(cols1)==len(cols2)==len(cols3)):
                print(f"Warning: mismatch in #lines for {rel}/{fn}", file=sys.stderr)

            # sum them
            for c1, c2, c3 in zip(cols1, cols2, cols3):
                total = extract_count(c1) + extract_count(c2) + extract_count(c3)
                c1.find('pre').string = format_number(total) if total >  0 else ""

            # write back into the merged output tree
            out_html = os.path.join(out_dir, rel, fn)
            with open(out_html, "w", encoding="utf8") as outf:
                outf.write(str(soup1))

    print("Merge complete!  See merged output in:", out_dir)


if __name__ == "__main__":
    p = argparse.ArgumentParser(
        description="Merge three lcov-style HTML coverage dirs by summing per-line counts"
    )
    p.add_argument("report1", help="first html coverage directory")
    p.add_argument("report2", help="second html coverage directory")
    p.add_argument("report3", help="third html coverage directory")
    p.add_argument("outdir",   help="destination for the merged coverage tree")
    args = p.parse_args()

    merge_reports(args.report1, args.report2, args.report3, args.outdir)
