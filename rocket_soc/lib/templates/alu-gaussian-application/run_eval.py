#!/usr/bin/env python3
import os
import re

approx_levels = [-1, 63, 62, 60, 56, 48, 32, 0]
image_files = [f for f in os.listdir('./image') if re.match(r'.*\.png', f)]

def remove_file_silent(file_name):
    try:
        os.remove(file_name)
    except OSError:
        pass 

def make_run():
    os.system("make clean 1>/dev/null && make run 1>/dev/null 2>/dev/null")

def create_config(mul, alu):
    remove_file_silent("approx_config.h")
    f = open("approx_config.h", "w")
    if (mul > 0):
        f.write("#define USE_MUL_APPROX\n")
    if (alu > 0):
        f.write("#define USE_ADD_APPROX\n")

    f.write("#define MUL_APPROX_AMOUNT " + str(mul) + "\n")
    f.write("#define ADD_APPROX_AMOUNT " + str(alu) + "\n")

    f.close()

def save_result(image, mul, alu):
    name = "" 

    if ((mul < 0) and (alu < 0)):
        name = "native"
    else:
        if (mul >= 0):
            name = name + "MUL" + str(mul)
        if (alu >= 0):
            name = name + "ADD" + str(alu)

    #print(name)
    os.system("cp result.png eval/" + str(image) + "/" + name + ".png")
    
os.system("rm -r eval")
os.system("mkdir -p eval")

for image in image_files:
    print("Starting run for '"+str(image)+"'...")
    # setup image file
    remove_file_silent("image.mak") 
    f = open("image.mak", "w")
    f.write("IMG_FILE=image/" + str(image))
    f.close()

    os.system("mkdir -p eval/" + str(image))

    # no alu
    for mul_neglect in approx_levels:
        for alu_neglect in approx_levels:
            create_config(mul_neglect, alu_neglect)
            make_run()
            save_result(image, mul_neglect, alu_neglect)
