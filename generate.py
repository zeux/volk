#!/usr/bin/python3

import sys
import urllib
import xml.etree.ElementTree as etree
import urllib.request

def parse_xml(path):
	file = urllib.request.urlopen(path) if path.startswith("http") else open(path, 'r')
	with file:
		tree = etree.parse(file)
		return tree

def patch_file(path, blocks):
	result = []
	with open(path, 'r') as file:
		lines = file.readlines()
		for line in lines:
			if line.strip().startswith('/* VOLK_GENERATE_'):
				for other in lines:
					if other == line:
						break
			else:
				result.append(line)

	with open(path, 'w') as file:
		for line in result:
			file.write(line)

if __name__ == "__main__":
	specpath = "https://raw.githubusercontent.com/KhronosGroup/Vulkan-Docs/master/src/spec/vk.xml"

	if len(sys.argv) > 1:
		specpath = sys.argv[1]

	spec = parse_xml(specpath)

	blocks = {}

	patch_file('volk.c', blocks)
	patch_file('volk.h', blocks)
