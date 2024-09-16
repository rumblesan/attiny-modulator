#!/usr/bin/env python

import sys
import math
import random

import argparse


def sine(args):
    delta = 360.0 / args.samplecount
    for i in range(args.samplecount):
        angle = i * delta
        rad = math.radians(angle)
        s = math.sin(rad)
        o = round((s * 127)) + 128
        print("  {}, // {}".format(o, i))


def saw(args):
    for i in range(args.samplecount):
        print("  {}, // {}".format(i, i))


def square(args):
    delta = 360.0 / args.samplecount
    for i in range(args.samplecount):
        if i < 128:
            o = 0
        else:
            o = 255
        print("  {}, // {}".format(o, i))


def rand(args):
    for i in range(args.samplecount):
        o = round((random.random() * args.samplecount))
        print("  {}, // {}".format(o, i))


def print_array_start(name, samplecount):
    print("const uint8_t {}[{}] PROGMEM = {{".format(name, samplecount))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="gentable.py", description="Creates the C code for the yorick synth wavetables"
    )
    parser.add_argument("wavetype", choices=["sine", "saw", "square", "rand"], default="sine")
    parser.add_argument("-s", "--samplecount", default=256, type=int)

    args = parser.parse_args()

    if args.wavetype == "sine":
        print_array_start("WT_SINE", args.samplecount)
        sine(args)
    elif args.wavetype == "saw":
        print_array_start("WT_SAW", args.samplecount)
        saw(args)
    elif args.wavetype == "square":
        print_array_start("WT_SQUARE", args.samplecount)
        square(args)
    elif args.wavetype == "rand":
        print_array_start("WT_RANDOM", args.samplecount)
        rand(args)
    print("};")
