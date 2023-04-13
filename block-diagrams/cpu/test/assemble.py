#!/usr/bin/env python3

from argparse import ArgumentParser
import os

parser = ArgumentParser(description="Assemble hack assembly and output in hex.")
parser.add_argument("file", help="Path to hack assembly file.")
args = parser.parse_args()

os.system(f"./HackAssembler {args.file}")
file_path = os.path.splitext(args.file)[0] + ".hack"
with open(file_path, "r") as infile:
    lines = infile.read().split("\n")

for line in lines[:-1]:
    asm = int(line, 2)
    print(f"{asm:08x}")

