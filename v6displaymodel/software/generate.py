# all colors are in R,G,B format
import numpy as np

TEMP_6500K = np.array((0.3, 0.59, 0.11))

BALANCE_WS2813 = np.array((360, 1150, 220))

FIRE_PALETTE = np.array((
    (0x00, 0x00, 0x00), #000000
    (0x0D, 0x01, 0x00), #0D0100
    (0x2D, 0x04, 0x01), #2D0401
    (0x4F, 0x09, 0x01), #4F0901
    (0x6E, 0x0E, 0x01), #6E0E01
    (0x8D, 0x12, 0x01), #8D1201
    (0xAB, 0x14, 0x01), #AB1401
    (0xC7, 0x15, 0x00), #C71500
    (0xDC, 0x1C, 0x00), #DC1C00
    (0xEB, 0x2C, 0x00), #EB2C00
    (0xF5, 0x3D, 0x00), #F53D00
    (0xFC, 0x4F, 0x00), #FC4F00
    (0xFF, 0x61, 0x00), #FF6100
    (0xFF, 0x73, 0x00), #FF7300
    (0xFF, 0x84, 0x05), #FF8405
    (0xFF, 0x94, 0x15), #FF9415
    (0xFF, 0xA1, 0x27), #FFA127
    (0xFF, 0xAD, 0x3A), #FFAD3A
    (0xFF, 0xB8, 0x4E), #FFB84E
    (0xFF, 0xC1, 0x63), #FFC163
    (0xFF, 0xCA, 0x78), #FFCA78
    (0xFF, 0xD2, 0x8C), #FFD28C
    (0xFF, 0xD9, 0xA0), #FFD9A0
    (0xFF, 0xDF, 0xB3), #FFDFB3
    (0xFF, 0xE4, 0xC5), #FFE4C5
    (0xFF, 0xEA, 0xD6), #FFEAD6
    (0xFF, 0xF0, 0xE3), #FFF0E3
    (0xFF, 0xF4, 0xEE), #FFF4EE
    (0xFF, 0xF8, 0xF6), #FFF8F6
    (0xFF, 0xFB, 0xFB), #FFFBFB
    (0xFF, 0xFD, 0xFE), #FFFDFE
    (0xFF, 0xFF, 0xFF), #FFFFFF
))

def determine_balanced_values(color_temp, device_balance):
    max_value = 0.0
    max_index = 0
    relative = color_temp / device_balance
    return relative / relative.max()

def convert_absolute_to_rgb(absolute, gamma=2.2):
    absolute = np.array(absolute)
    rgb = (absolute / 0xFF) ** (1 / gamma)
    return [int(round(i * 0xFF)) for i in rgb]

def convert_rgb_to_absolute(rgb, gamma=2.2):
    rgb = np.array(rgb)
    absolute = (rgb / 0xFF) ** gamma
    return [int(round(i * 0xFF)) for i in absolute]

def main():
    balance = determine_balanced_values(TEMP_6500K, BALANCE_WS2813)
    balance_8bit = [int(round(i * 0xFF)) for i in balance]
    print(f"6500k white balance: R=0x{balance_8bit[0]:02X}, G=0x{balance_8bit[1]:02X}, B=0x{balance_8bit[2]:02X}")

    new_palette = FIRE_PALETTE * balance
    print("new palette:")
    for color in new_palette:
        color = [int(round(i)) for i in color]
        print(f"    Pixel(0x{color[0]:02X}, 0x{color[1]:02X}, 0x{color[2]:02X}),")



if __name__ == '__main__':
    main()
