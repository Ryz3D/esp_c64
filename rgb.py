# hex color -> uint16_t
# python rgb.py #ffffff #000000

import sys

def conv(bits, arg):
    return bin(round((2 ** bits - 1) * int(arg, 16) / 255))[2:].zfill(bits)

print(f"color_bright = (0b{conv(5, sys.argv[1][1:3])} << 11) | (0b{conv(6, sys.argv[1][3:5])} << 5) | 0b{conv(5, sys.argv[1][5:7])};")
print(f"color_dark = (0b{conv(5, sys.argv[2][1:3])} << 11) | (0b{conv(6, sys.argv[2][3:5])} << 5) | 0b{conv(5, sys.argv[2][5:7])};")
