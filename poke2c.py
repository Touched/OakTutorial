#!/usr/bin/python3
import json
import os
import sys

file = sys.argv[1]

matches = {'[Lv]': 52, 'B': 188, ' ': 0, '.': 173, 'ú': 39, 'u': 233, 'F': 192, '\\c': 252, 'ô': 36, 'º': 42, '[u]': 247, '5': 166, '\\l': 250, 'ù': 38, 'È': 5, '$': 183, 'Z': 212, 't': 232, 'Ú': 18, 'C': 189, '9': 170, 's': 231, 'h': 220, 'E': 191, 'Ì': 9, 'Ï': 12, 'a': 213, 'Ö': 242, 'd': 216, '[K]': 89, '[MN]': 84, '\\v': 253, 'Ò': 13, ')': 93, 'M': 199, 'Î': 11, 'I': 195, '["]': 178, 'x': 236, 'Ä': 241, 'ï': 33, 'b': 214, 'Q': 203, 'm': 225, 'Ô': 15, '[OC]': 88, 'ü': 246, 'á': 23, ':': 240, 'v': 234, 'L': 198, 'j': 222, 'ä': 244, '[Ke]': 86, '[PO]': 85, 'Ê': 7, 'R': 204, 'K': 197, 'Ü': 243, 'ñ': 41, '[m]': 181, '"': 177, "'": 179, 'Ë': 8, 'À': 1, '[PK]': 83, '\\p': 251, 'Ù': 17, '¿': 81, '\\x': 255, '7': 168, 'ö': 245, '[f]': 182, '[BL]': 87, 'n': 226, 'Á': 2, 'ò': 34, 'è': 26, '&': 45, '8': 169, 'ó': 35, 'T': 206, 'g': 219, '[L]': 123, 'Í': 90, 'G': 193, 'Û': 19, 'e': 217, ';': 54, '-': 174, 'à': 22, 'z': 238, '/': 186, 'P': 202, 'N': 200, 'Ç': 4, 'ê': 28, 'æ': 37, 'ç': 25, 'Ó': 14, 'í': 111, 'A': 187, '+': 46, '[...]': 176, '[d]': 248, 'ß': 21, '0': 161, 'D': 190, 'É': 6, '1': 162, 'f': 218, 'i': 221, 'p': 228, 'w': 235, 'H': 194, '·': 175, '4': 165, 'Â': 3, ',': 184, '[R]': 124, 'î': 32, 'X': 210, '\\n': 254, 'â': 104, '[x]': 185, 'W': 209, 'Y': 211, 'û': 40, '?': 172, 'q': 229, 'ë': 29, 'Ñ': 20, 'V': 208, '[l]': 249, 'é': 27, 'J': 196, '[>]': 239, 'l': 224, 'c': 215, 'r': 230, '3': 164, '[D]': 122, '!': 171, '(': 92, "[']": 180, 'U': 207, 'k': 223, '2': 163, 'Æ': 16, 'S': 205, '6': 167, 'ª': 43, 'y': 237, '¡': 82, 'o': 227, 'O': 201, '=': 53, 'ì': 30, '%': 91}

def convert(string):
    # Escape the escape chars, setup input and output
    string = repr(string)[1:-1]
    output = bytearray()

    # Replacement for fake escape chars
    # TODO: Probably should change this to a regex matching \p, \x, \v, etc.
    string = string.replace('\\\\', '\\')

    # Loop through characters like this for fine control over step
    i = 0
    while i < len(string):
        # What's left to be processed
        sub = string[i:]

        # Closure to determine longest matching substring
        def longest(s):
            for j in range(len(s)):
                if sub[j] != s[j]:
                    return j
            return len(s)

        # Sort matches, choose the largest match
        longest = sorted(matches.keys(), key=longest)[-1]

        # Step forward the length of match, and append output
        i += len(longest)
        output.append(matches[longest])

    # Sentinel
    output.append(0xFF)
    return output


base, _ = os.path.splitext(file)
guard = 'STRINGS_' + base.upper() + '_H_'
header = '#ifndef {0}\n#define {0}\n\n'.format(guard)
with open(file) as fp:
    for line in fp:
        line = line.strip()

        # Ignore blank lines
        if not line:
            continue

        name, value = line.split('\t')
        data = convert(value)

        arr = ', '.join(hex(byte) for byte in data)

        symbol = 'char {}[] = {{ {} }};'.format(name, arr)

        header += symbol + '\n\n'

header += '#endif'

with open(base + '.h', 'w') as fp:
    print(header, file=fp)




