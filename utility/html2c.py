# -*- coding: utf-8 -*-
"""
Created on Fri Mar 17 10:44:40 2023

@author: user
"""

import os
import binascii
import gzip

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

source_dir = '..\\html'
target_dir = '..\\firmware\\MMSL'
gz_dir = '..\\html\\gz_html'

if not os.path.exists(gz_dir):
	os.makedirs(gz_dir)
	
for html_filename in os.listdir(source_dir):
	html_path_filename = os.path.join(source_dir, html_filename)
	html_file_stats = os.stat(html_path_filename)
	
	# checking if it is a file
	if os.path.isfile(html_path_filename):
		
		# checking if html or css file
		html_filename_split = html_filename.split(".")
		if (html_filename_split[1] != "html"):
			if (html_filename_split[1] != "css"):
				continue
		
		if (html_filename_split[1] == "css"):
			header_filename = html_filename_split[0] + ".h"
			header_path_filename = os.path.join(target_dir, header_filename)
		
			if os.path.isfile(header_path_filename):
				file_header_stats = os.stat(header_path_filename)
				if (file_header_stats.st_mtime >= html_file_stats.st_mtime):
					continue
		
			print('translate %s to %s' % (html_path_filename, header_path_filename))
			with open(html_path_filename, 'rb') as html_file_object:
				html_content = html_file_object.read()
			hex_data = binascii.hexlify(html_content)
			hex_array = hex2c(hex_data)
			array_name = "const unsigned char " + html_filename_split[0] + "_hex[] = {\n"
			f = open(header_path_filename, "w")
			f.write(array_name)
			f.write(hex_array)
			f.write("\n};\n")
			f.close()
			continue

		if (html_filename_split[1] == "html"):
			gz_header_filename = html_filename_split[0] + ".gz.h"
			gz_header_path_filename = os.path.join(target_dir, gz_header_filename)
			
			if os.path.exists(gz_header_path_filename):
				gz_header_file_stats = os.stat(gz_header_path_filename)
				if (gz_header_file_stats.st_mtime >= html_file_stats.st_mtime):
					continue

			gz_html_filename = html_filename_split[0] + "." + html_filename_split[1] + ".gz"
			gz_html_path_filename = os.path.join(gz_dir, gz_html_filename)
			print('gzip %s to %s' % (html_path_filename, gz_html_path_filename))
			print(f'File Size before zip: {html_file_stats.st_size}')
			with open(html_path_filename, 'rb') as html_file_object:
				html_content = html_file_object.read()
			with gzip.open(gz_html_path_filename, "wb") as zip_file_object:
				zip_file_object.write(html_content)
			zip_file_stats = os.stat(gz_html_path_filename)
			print(f'File Size after zip: {zip_file_stats.st_size}')
			
			print('translate %s to %s' % (html_path_filename, gz_header_path_filename))
			with open(gz_html_path_filename, 'rb') as zip_file_object:
				gzip_html_context = zip_file_object.read()
			hex_data = binascii.hexlify(gzip_html_context)
			hex_array = hex2c(hex_data)
			fz = open(gz_header_path_filename, "w")
			fz.write("const unsigned char " + html_filename_split[0] + "_hex[] = {\n")
			fz.write(hex(html_file_stats.st_size & 0x00FF) + ", ")
			fz.write(hex((html_file_stats.st_size & 0xFF00) >> 8) + ", \n")
			fz.write(hex_array)
			fz.write("\n};\n")
			fz.close()
		

