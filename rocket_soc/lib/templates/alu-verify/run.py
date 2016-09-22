#!/usr/bin/python3
import os

for i in range(1,10000):
    print("Running Test" + str(i) + "...")
    os.system("make clean")
    os.system("make run >> result.txt")
