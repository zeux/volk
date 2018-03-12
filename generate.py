#!/usr/bin/python3

from collections import OrderedDict
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
	block = None

	with open(path, 'r') as file:
		for line in file.readlines():
			if block:
				if line == block:
					result.append(line)
					block = None
			else:
				result.append(line)
				if line.strip().startswith('/* VOLK_GENERATE_'):
					block = line
					result.append(blocks[line.strip()[17:-3]])

	with open(path, 'w') as file:
		for line in result:
			file.write(line)

def is_descendant_type(spec, name, base):
	if name == base:
		return True
	type = spec.find('types/type[name="' + name + '"]')
	if not type:
		return False
	parent = type.get('parent')
	if not parent:
		return False
	return is_descendant_type(spec, parent, base)

if __name__ == "__main__":
	specpath = "https://raw.githubusercontent.com/KhronosGroup/Vulkan-Docs/master/src/spec/vk.xml"

	if len(sys.argv) > 1:
		specpath = sys.argv[1]

	spec = parse_xml(specpath)

	blocks = {}
	blocks['DEVICE_TABLE'] = ''
	blocks['PROTOTYPES_H'] = ''
	blocks['PROTOTYPES_C'] = ''
	blocks['LOAD_LOADER'] = ''
	blocks['LOAD_INSTANCE'] = ''
	blocks['LOAD_DEVICE'] = ''
	blocks['LOAD_DEVICE_TABLE'] = ''

	version = spec.find('types/type[name="VK_HEADER_VERSION"]')
	blocks['VERSION'] = '#define VOLK_HEADER_VERSION ' + version.find('name').tail.strip() + '\n'

	command_groups = OrderedDict()

	for feature in spec.findall('feature'):
		command_groups[feature.get('name')] = feature.findall('require/command')

	for ext in sorted(spec.findall('extensions/extension'), key=lambda ext: ext.get('name')):
		name = ext.get('name')
		for req in ext.findall('require'):
			key = name if not req.get('feature') else name + ' && ' + req.get('feature')
			commands = req.findall('command')
			if command_groups.get(key):
				command_groups[key] += commands
			else:
				command_groups[key] = commands

	commands = {}

	for cmd in spec.findall('commands/command'):
		if not cmd.get('alias'):
			name = cmd.findtext('proto/name')
			commands[name] = cmd

	for cmd in spec.findall('commands/command'):
		if cmd.get('alias'):
			name = cmd.get('name')
			commands[name] = commands[cmd.get('alias')]

	for (group, cmdrefs) in command_groups.items():
		ifdef = '#if ' + group + '\n'

		for key in ('LOAD_DEVICE', 'LOAD_DEVICE_TABLE', 'DEVICE_TABLE', 'LOAD_INSTANCE', 'LOAD_LOADER', 'PROTOTYPES_H', 'PROTOTYPES_C'):
			blocks[key] += ifdef

		for cmdref in sorted(cmdrefs, key=lambda cmdref: cmdref.get('name')):
			name = cmdref.get('name')
			cmd = commands[name]
			type = cmd.findtext('param[1]/type')

			if name == 'vkGetInstanceProcAddr':
				type = ''
			if name == 'vkGetDeviceProcAddr':
				type = 'VkInstance'

			if is_descendant_type(spec, type, 'VkDevice'):
				blocks['LOAD_DEVICE'] += '\t' + name + ' = (PFN_' + name + ')load(context, "' + name + '");' + "\n"
				blocks['LOAD_DEVICE_TABLE'] += '\ttable->' + name + ' = (PFN_' + name + ')load(context, "' + name + '");' + "\n"
				blocks['DEVICE_TABLE'] += '\tPFN_' + name + ' ' + name + ";\n"
			elif is_descendant_type(spec, type, 'VkInstance'):
				blocks['LOAD_INSTANCE'] += '\t' + name + ' = (PFN_' + name + ')load(context, "' + name + '");' + "\n"
			elif type != '':
				blocks['LOAD_LOADER'] += '\t' + name + ' = (PFN_' + name + ')load(context, "' + name + '");' + "\n"

			blocks['PROTOTYPES_H'] += 'extern PFN_' + name + ' ' + name + ";\n"
			blocks['PROTOTYPES_C'] += 'PFN_' + name + ' ' + name + ";\n"

		for key in ('LOAD_DEVICE', 'LOAD_DEVICE_TABLE', 'DEVICE_TABLE', 'LOAD_INSTANCE', 'LOAD_LOADER', 'PROTOTYPES_H', 'PROTOTYPES_C'):
			if blocks[key].endswith(ifdef):
				blocks[key] = blocks[key][:-len(ifdef)]
			else:
				blocks[key] += '#endif // ' + group + '\n'

	patch_file('volk.h', blocks)
	patch_file('volk.c', blocks)
