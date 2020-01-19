#!/usr/bin/env python
"""
Outputs given directory contents as a 'lump':

	lump ID: long
	directory offset: long

	<data for items>: x bytes

	directory ID: long
	number of items: long

		item ID: long
		item data offset: long
		item data length: long
		item name length: long
		<item name>

- 'long' is 4 byte signed big endian integer
- offsets are counted from the lump start
- item names are terminated with NULL byte
- beginning of item data is always long aligned

(CVS directories and backup files are ignored)


usage: dumplump  <path1> [<path2>...] >  <lump>

dumps contents of 'path' directories into 'lump' file.

(w) 2000 by Eero Tamminen <t150315@cc.tut.fi>
"""

import os
import sys
import struct


# NiL lump IDs
id = 0xbabe0000
header_lump = id+1
header_dir  = id+2
header_item = id+3
header_rgbf = id+4


class Lump:

    # subdirectories to ignore
    ignore_dirs = ['CVS']
    # extensions for files to ignore
    ignore_exts = ['~', '.bak']


    def __init__(self, dirs):
	self.items = []
	cwd = os.getcwd()
	for dir in dirs:
	    os.chdir(dir)
	    self.dir = dir
	    sys.stderr.write("reading filenames in '%s'...\n" % dir)
	    os.path.walk('.', self.storedir, None)
	    os.chdir(cwd)
	self.items.sort()


    def storedir(self, dummy, dir, names):
	if os.path.basename(dir) in self.ignore_dirs:
	    return
	for name in names:
	    self.storefile(dir[2:], name)


    def storefile(self, dir, name):
	for ext in self.ignore_exts:
	    if name[-len(ext):] == ext:
		return
	name = dir + '/' + name
	if os.path.isfile(name):
	    self.items.append((self.dir, name))


    def list(self, output):
	for item in self.items:
	    output.write(item[1] + '\n')


    def dump(self, output):

	stat = os.stat
	pack = struct.pack
	out = output.write
	log = sys.stderr.write

	log("dumping directory contents...\n")

	# after lump header
	offset = 2 * 4

	sizes = []
	offsets = []
	paddings = []
	# get file sizes and offsets
	for item in self.items:
	    size = stat(item[0] + '/' + item[1])[6]
	    sizes.append(size)
	    offsets.append(offset)

	    # data long alignment
	    extra = offset & 3
	    if extra:
		extra = 4 - extra
	    offset = offset + size + extra
	    paddings.append(extra)

	# output lump header, dir offset
	out(pack('>LL', header_lump, offset))

	idx = 0
	# output file contents
	for item in self.items:
	    file = item[0] + '/' + item[1]
	    log(file + '\n')
	    out(open(file).read())
	    if paddings[idx]:
		out('\0' * paddings[idx])

	    # re-check file size
	    size = stat(file)[6]
	    if size != sizes[idx]:
		log("ERROR: file '%s' size changed while dumping the lump" % file)
		sys.exit(-1)
	    idx = idx + 1

	# output dir header, dir entries
	out(pack('>LL', header_dir, len(self.items)))

	idx = 0
	# output dir entries
	for item in self.items:
	    name = item[1]
	    # item header, file offset, file size, name size
	    out(pack('>LLLL', header_item, offsets[idx], sizes[idx], len(name)+1))
	    out(name + '\0')
	    idx = idx + 1


if __name__ == '__main__':
    if len(sys.argv) > 1:
	Lump(sys.argv[1:]).dump(sys.stdout)
    else:
	print __doc__
