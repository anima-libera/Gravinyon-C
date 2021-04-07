#!/usr/bin/env python3

""" Compiles the Gravinyon source code.

Usage:
  {this_script} [options]

Options:
  -h  --help        Prints this docstring.
  -l  --launch      Executes the bin if compiled, with what follows as args.
  -d  --debug       Standard debuging build, defines DEBUG, launches with -d.

Example usage for debug:
  {this_script} -d -l
"""

import sys
import os
import re

def print_blue(*args, **kwargs):
	print("\x1b[36m", end = "")
	print(*args, **kwargs)
	print("\x1b[39m", end = "", flush = True)

# Launch option -l
if "-l" in sys.argv[1:]:
	option_launch = True
	i = sys.argv[1:].index("-l")
elif "--launch" in sys.argv[1:]:
	option_launch = True
	i = sys.argv[1:].index("--launch")
else:
	option_launch = False
if option_launch:
	options = sys.argv[1:i+1]
	launch_args = sys.argv[i+2:]
else:
	options = sys.argv[1:]

# Options
def cmdline_has_option(*option_names):
	for option_name in option_names:
		if option_name in options:
			return True
	return False
option_help = cmdline_has_option("-h", "--help")
option_debug = cmdline_has_option("-d", "--debug")
release_build = not option_debug
src_dir_name = "src"
bin_dir_name = "bin"
bin_name = "Gravinyon"

# Help message if -h
if option_help:
	this_script = sys.argv[0]
	python = "" if this_script.startswith("./") else "python3 "
	print(__doc__.strip().format(this_script = python + sys.argv[0]))
	sys.exit(0)

# Embed content
embedded_header_file_name = "embedded.h" # See this file for some explanations
embedded_source_file_name = "embedded.c"
embed_re = r"EMBEDDED\s*\(\s*\"([^\"]+)\"\s*\)\s*([^\s][^;]+[^\s])\s*;"
def escaped_file_content(filepath):
	if option_debug:
		print("Embed file \"{}\"".format(filepath))
	try:
		with open(filepath, "r") as file:
			return file.read().translate({
				ord("\""): "\\\"", ord("\\"): "\\\\",
				ord("\n"): "\\n", ord("\t"): "\\t"})
	except FileNotFoundError as error:
		print("\x1b[31mEmbedded file error:\x1b[39m " +
			"The embedded content generator couldn't find the file " +
			"\"{}\" used in an EMBEDDED macro in the ".format(filepath) +
			"\"{}\" header file.".format(embedded_header_file_name))
		raise error
em_content = []
with open(os.path.join(src_dir_name, embedded_header_file_name), "r") as ehf:
	for match in re.finditer(embed_re, ehf.read(), flags = re.MULTILINE):
		em_content.append("/* Content of \"{}\". */".format(match.group(1)))
		em_content.append("{} = \"{}\";".format(match.group(2),
			escaped_file_content(src_dir_name + "/" + match.group(1))))
		em_content.append("")
with open(os.path.join(src_dir_name, embedded_source_file_name), "w") as esf:
	esf.write("\n")
	esf.write("/* This file is overwritten at each compilation.\n")
	esf.write(" * Do not modify, see \"{}\" ".format(
		embedded_header_file_name))
	esf.write("or \"_comp.py\" instead. */\n\n")
	esf.write("\n".join(em_content)) # There is a trailing newline it's ok

# List src files
src_file_names = []
for dir_name, _, file_names in os.walk(src_dir_name):
	for file_name in file_names:
		if file_name.split(".")[-1] == "c":
			src_file_names.append(os.path.join(dir_name, file_name))

# Bin directory
if not os.path.exists(bin_dir_name):
	os.makedirs(bin_dir_name)

# Build
build_command_args = ["gcc"]
for src_file_name in src_file_names:
	build_command_args.append(src_file_name)
build_command_args.append("-o")
build_command_args.append(os.path.join(bin_dir_name, bin_name))
build_command_args.append("-I" + src_dir_name)
build_command_args.append("-std=c11")
build_command_args.append("-Wall")
build_command_args.append("-Wextra")
build_command_args.append("-pedantic")
build_command_args.append("-Wno-overlength-strings")
if option_debug:
	build_command_args.append("-DDEBUG")
	build_command_args.append("-g")
if release_build:
	build_command_args.append("-O2")
	build_command_args.append("-fno-stack-protector")
	build_command_args.append("-flto")
build_command_args.append("-lGL")
build_command_args.append("-DGLEW_STATIC")
build_command_args.append("-lGLEW")
build_command_args.append("`sdl2-config --cflags --libs`") # See the SDL2 doc
build_command_args.append("-lm")
build_command = " ".join(build_command_args)
print_blue(build_command)
build_exit_status = os.system(build_command)

# Launch if -l
if option_launch and build_exit_status == 0:
	launch_command_args = ["./" + bin_name]
	for launch_arg in launch_args:
		launch_command_args.append(launch_arg)
	if option_debug:
		launch_command_args.append("-d")
	launch_command = " ".join(launch_command_args)
	os.chdir(bin_dir_name)
	print_blue(launch_command)
	launch_exit_status_raw = os.system(launch_command)
	launch_exit_status = launch_exit_status_raw >> 8
	if bin_dir_name != ".":
		os.chdir("..")
	if launch_exit_status != 0:
		print_blue("exit status {}".format(launch_exit_status))
