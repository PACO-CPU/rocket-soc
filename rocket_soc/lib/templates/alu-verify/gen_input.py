#!/usr/bin/env python

import random
import os

input_size = random.randint(1, 20000);

print("#include <stdint.h>")
print("#define INPUT_SIZE " + str(input_size))
print("uint64_t input_a[INPUT_SIZE] = {")

for i in range(1, input_size):
    val = random.randint(0, (1 << 32) - 1)
    print(str(val) + "UL,")

print("};")

print("uint64_t input_b[INPUT_SIZE] = {")

for i in range(1, input_size):
    val = random.randint(0, (1 << 32) - 1)
    print(str(val) + "UL,")

print("};")
