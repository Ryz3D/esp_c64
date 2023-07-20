# uint16_t -> hex color
# python antirgb.py

color_bright0 = (0b11001 << 11) | (0b111000 << 5) | 0b11111
color_dark0 = (0b00101 << 11) | (0b001000 << 5) | 0b11010

color_bright1 = (0b01011 << 11) | (0b100011 << 5) | 0b01000
color_dark1 = (0b00001 << 11) | (0b001010 << 5) | 0b00100

color_bright2 = (0b00001 << 11) | (0b001010 << 5) | 0b00100
color_dark2 = (0b01011 << 11) | (0b100011 << 5) | 0b01000

color_bright3 = (0b11111 << 11) | (0b111111 << 5) | 0b11111
color_dark3 = (0b00000 << 11) | (0b000000 << 5) | 0b00000

color_bright4 = (0b00000 << 11) | (0b000000 << 5) | 0b00000
color_dark4 = (0b11111 << 11) | (0b111111 << 5) | 0b11111

color_bright5 = (0b01010 << 11) | (0b110000 << 5) | 0b11111
color_dark5 = (0b00100 << 11) | (0b001000 << 5) | 0b00100

def get_comp(bits):
    return [((bits >> 11) & 0b11111) / 31, ((bits >> 5) & 0b111111) / 63, (bits & 0b11111) / 31]

def get_html(comp):
    return "".join([hex(round(c * 255))[2:].zfill(2) for c in comp])

print(f"{get_html(get_comp(color_bright0))} {get_html(get_comp(color_dark0))}")
print(f"{get_html(get_comp(color_bright1))} {get_html(get_comp(color_dark1))}")
print(f"{get_html(get_comp(color_bright2))} {get_html(get_comp(color_dark2))}")
print(f"{get_html(get_comp(color_bright3))} {get_html(get_comp(color_dark3))}")
print(f"{get_html(get_comp(color_bright4))} {get_html(get_comp(color_dark4))}")
print(f"{get_html(get_comp(color_bright5))} {get_html(get_comp(color_dark5))}")
