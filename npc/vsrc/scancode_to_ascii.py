# -*- coding: utf-8 -*-

scancode_to_ascii = {
    0x0E: '`',
    0x16: '1',
    0x1E: '2',
    0x26: '3',
    0x25: '4',
    0x2E: '5',
    0x36: '6',
    0x3D: '7',
    0x3E: '8',
    0x46: '9',
    0x45: '0',
    0x4E: '-',
    0x55: '=',
    0x5D: '\\',
    0x66: '\b', # Backspace
    0x0D: '\t', # Tab
    0x15: 'q',
    0x1D: 'w',
    0x24: 'e',
    0x2D: 'r',
    0x2C: 't',
    0x35: 'y',
    0x3C: 'u',
    0x43: 'i',
    0x44: 'o',
    0x4D: 'p',
    0x54: '[',
    0x5B: ']',
    0x5A: '\n', # Enter
    0x1C: 'a',
    0x1B: 's',
    0x23: 'd',
    0x2B: 'f',
    0x34: 'g',
    0x33: 'h',
    0x3B: 'j',
    0x42: 'k',
    0x4B: 'l',
    0x4C: ';',
    0x52: '\'',
    0x1A: 'z',
    0x22: 'x',
    0x21: 'c',
    0x2A: 'v',
    0x32: 'b',
    0x31: 'n',
    0x3A: 'm',
    0x41: ',',
    0x49: '.',
    0x4A: '/',
    0x29: ' ', # Space
}

with open("scancode_to_ascii.txt", "w") as file:
    for i in range(256):
        if i in scancode_to_ascii:
            file.write(f"{ord(scancode_to_ascii[i]):02x}\n")
        else:
            file.write(f"00\n")
