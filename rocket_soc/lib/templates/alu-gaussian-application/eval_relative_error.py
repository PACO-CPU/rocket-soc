#!/usr/bin/env python3
import os
from PIL import Image
import argparse

approx_levels = [-1, 63, 62, 60, 56, 48, 32, 0] 
def relative_error(imageP, imageA):
    pixel_sum = 0    

    for i in range(len(imageP)):
        pixel_sum = pixel_sum + abs(imageP[i] - imageA[i])/imageP[i]
    return float(pixel_sum) / float(len(imageP))                                                                                        

for folder in os.listdir("eval"):
    # load native image
    nativeImage = Image.open("eval/" + str(folder) + "/native.png")
    nativeData = list(nativeImage.getdata())
    # prepare csv
    f = open("eval/" + str(folder) + "/error.csv", "w")
    f.write("MUL neglect, ADD neglect, Relative error\n")

    # write the results
    for mul in approx_levels:
        for add in approx_levels:
            if (mul < 0 and add < 0): 
                continue
            if (mul < 0):
                mul_path = ""
            else:
                mul_path = "MUL"+str(mul)

            if (add < 0):
                add_path = ""
            else:
                add_path = "ADD"+str(add)
            imageApprox = Image.open("eval/" + str(folder) + "/" + mul_path + add_path + ".png")
            dataApprox = list(imageApprox.getdata())
            f.write(str(mul) + "," + str(add) + "," +str(relative_error(nativeData, dataApprox)) + "\n")

    f.close()

#image1 = Image.open(args.ip[0])
#image2 = Image.open(args.ia[0])


#data1 = list(image1.getdata())
#data2 = list(image2.getdata())

#print(relative_error(data1, data2))

