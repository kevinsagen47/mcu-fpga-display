# -*- coding: utf-8 -*-
"""
Created on Sat Feb 18 16:45:10 2023

@author: user
"""

import binascii

def hex2c(hex_data):
	hex_head = 1
	new_line = 0
	hex_array = ""
	for c in hex_data:
		if (hex_head == 1):
			hex_array = hex_array + "0x" + chr(c)
			hex_head = 0
		else:
			hex_array = hex_array + chr(c) + ", "
			hex_head = 1
			new_line = new_line + 1
		if (new_line == 16):
			hex_array = hex_array + "\n"
			new_line = 0
	return hex_array

favicon_path_filename = '..\\favicon\\favicon.ico'
header_path_filename = '..\\firmware\\MMSL\\html_itri_favicon.h'

with open(favicon_path_filename, 'rb') as f:
	content = f.read()
hex_data = binascii.hexlify(content)
hex_array = hex2c(hex_data)
array_name = "const char favicon_hex[] = {\n"
f = open(header_path_filename, "w")
f.write(array_name)
f.write(hex_array)
f.write("\n};\n")
f.close()
