#!/usr/bin/env python2

import os, sys
import argparse
from PIL import Image

# argument parser
parser = argparse.ArgumentParser(description="'Image to Raw' & 'Raw to Image' converter")
parser.add_argument('--image', action='store_true', help='convert a raw C-data-array to an image')
parser.add_argument('--raw', action='store_true', help='convert an image to a raw C-data-array')
parser.add_argument('-i', nargs=1, required=True, help='path to the input file')
parser.add_argument('-o', nargs=1, required=True, help='path to the output file')
args = parser.parse_args()

# raw -> image
if args.image:
    f = open(args.i[0], 'r')
    data = f.read()
    f.close()


    if not "//(" in data:
        print("Error: Raw data malformed. Expected image ratio")
        exit(1)

    # determine image ration
    ws = int(data.find("//(")) + 3
    we = int(data.find(",", ws))
    width = int(data[ws:we])

    hs = we + 1
    he = int(data.find(")", hs))
    height = int(data[hs:he])

    if data.find("{"):
        data = data.split("{")[1]
        data = data.split("}")[0]
        data = data.split(",")
        data = map(lambda x: x.replace("\n", ""), data[:-1])
        data_int = map(int, data)

        img = Image.new("L", (width, height))
        img.putdata(data_int)
        # fix up the image data
        img = img.rotate(-90)
        img = img.transpose(Image.FLIP_LEFT_RIGHT)
        # save it to a file
        img.save(args.o[0])

    else:
        print("Error: Raw data malformed")

# image -> raw
elif args.raw:
    try:
        img = Image.open(args.i[0])
        img.load()
        width, height = img.size
    except:
        print("Unable to load image: '" + args.i[0] + "'")
        exit(1)
 
    data = list(img.getdata())
    
    # write the result to the output-file
    f = open(args.o[0], 'w')
    f.write("//(" + str(width) + ", " + str(height) + ")\n")
    f.write("int approx(neglect_amount=2 inject=1 relax=1) image[] = {\n")
    for v in data:
        f.write(str(v[0]) + ",\n")
    f.write("};")

    f.close
