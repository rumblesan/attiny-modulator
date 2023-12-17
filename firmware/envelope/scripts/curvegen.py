#!/usr/bin/env python

import matplotlib.pyplot as plt
import sys
import math
import random

import argparse

def linear(args):
    return [i for i in range(args.samplecount)]

def quadratic(args):
    mult = 1.0 / (args.samplecount - 1)
    values = []
    for i in range(args.samplecount):
        v = float(i) * mult
        o = round(255.0 * v * v)
        values.append(o)
    return values

def exponential(args):
    mult = 1.0 / (args.samplecount - 1)
    values = []
    for i in range(args.samplecount):
        v = i * mult
        o = round(math.pow(256, v)) - 1
        values.append(o)
    return values

def sine(args):
    delta = 180.0 / args.samplecount
    values = []
    for i in range(args.samplecount):
        angle = (i * delta) - 90.0
        rad = math.radians(angle)
        s = math.sin(rad)
        o = round((s * 127)) + 128
        values.append(o)
    return values

def inverse_sine(args):
    delta = 180.0 / args.samplecount
    values = []
    for i in range(round(args.samplecount / 2)):
        angle = (i * delta)
        rad = math.radians(angle)
        s = math.sin(rad)
        o = round(s * 127)
        values.append(o)
    for i in range(round(args.samplecount / 2)):
        angle = (i * delta) - 90.0
        rad = math.radians(angle)
        s = math.sin(rad)
        o = round((s * 127)) + 255
        values.append(o)
    return values

def sharper(args):
    m = 800.0
    delt = m / args.samplecount
    values = [(m / (args.samplecount - i) - delt) for i in range(args.samplecount)]
    return list(map(lambda x: round(x) if x < 256 else 255, values))

def inverse(values):
    return list(reversed([255 - i for i in values]))

def print_env_curve_array(name, values, samplecount):
    print("const uint8_t {}[{}] PROGMEM = {{".format(name, samplecount))
    for idx, val in enumerate(values):
        print("  {}, // {}".format(val, idx))
    print("};")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="gentable.py", description="Creates the C code for the envelope curve tables"
    )
    parser.add_argument("envtype", choices=["linear", "quadratic", "invquadratic", "exponential", "invexponential", "sine", "invsine", "img", "all"], default="linear")
    parser.add_argument("-s", "--samplecount", default=256, type=int)

    args = parser.parse_args()

    if args.envtype == "linear":
        values = linear(args)
        print_env_curve_array("ENV_LINEAR", values, args.samplecount)
    elif args.envtype == "quadratic":
        values = quadratic(args)
        print_env_curve_array("ENV_QUADRATIC", values, args.samplecount)
    elif args.envtype == "invquadratic":
        values = inverse(quadratic(args))
        print_env_curve_array("ENV_INV_QUADRATIC", values, args.samplecount)
    elif args.envtype == "exponential":
        values = exponential(args)
        print_env_curve_array("ENV_EXPONENTIAl", values, args.samplecount)
    elif args.envtype == "invexponential":
        values = inverse(exponential(args))
        print_env_curve_array("ENV_INV_EXPONENTIAl", values, args.samplecount)
    elif args.envtype == "sharper":
        values = sharper(args)
        print_env_curve_array("ENV_SHARPER", values, args.samplecount)
    elif args.envtype == "invsharper":
        values = inverse(sharper(args))
        print_env_curve_array("ENV_INV_SHARPER", values, args.samplecount)
    elif args.envtype == "sine":
        values = sine(args)
        print_env_curve_array("ENV_SINE", values, args.samplecount)
    elif args.envtype == "invsine":
        values = inverse_sine(args)
        print_env_curve_array("ENV_INV_SINE", values, args.samplecount)
    elif args.envtype == "img":
        x = [i for i in range(0, 256)]
        plt.figure(figsize=(10, 10))
        plt.plot(x, linear(args), label="linear")
        plt.plot(x, quadratic(args), label="quadratic")
        plt.plot(x, inverse(quadratic(args)), label="inverse quadratic")
        plt.plot(x, exponential(args), label="exponential")
        plt.plot(x, inverse(exponential(args)), label="inverse exponential")
        # plt.plot(x, sine(args), label="sine")
        # plt.plot(x, inverse_sine(args), label="inverse sine")
        plt.plot(x, sharper(args), label="sharper")
        plt.plot(x, inverse(sharper(args)), label="inverse sharper")
        plt.legend()
        plt.show()
    elif args.envtype == "all":
        values = linear(args)
        print_env_curve_array("ENV_LINEAR", values, args.samplecount)
        print("\n")
        values = quadratic(args)
        print_env_curve_array("ENV_QUADRATIC", values, args.samplecount)
        print("\n")
        values = inverse(quadratic(args))
        print_env_curve_array("ENV_INV_QUADRATIC", values, args.samplecount)
        print("\n")
        values = exponential(args)
        print_env_curve_array("ENV_EXPONENTIAL", values, args.samplecount)
        print("\n")
        values = inverse(exponential(args))
        print_env_curve_array("ENV_INV_EXPONENTIAL", values, args.samplecount)
        print("\n")
        values = sharper(args)
        print_env_curve_array("ENV_SHARPER", values, args.samplecount)
        print("\n")
        values = inverse(sharper(args))
        print_env_curve_array("ENV_INV_SHARPER", values, args.samplecount)
        print("\n")

