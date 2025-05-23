#!/usr/bin/env python3

# Copyright (C) 2017-2025 Free Software Foundation, Inc.
#
# Checks some of the GNU style formatting rules in a set of patches.
# The script is a rewritten of the same bash script and should eventually
# replace the former script.
#
# This file is part of GCC.
#
# GCC is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 3, or (at your option) any later
# version.
#
# GCC is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License
# along with GCC; see the file COPYING3.  If not see
# <http://www.gnu.org/licenses/>.

import argparse
import sys
from check_GNU_style_lib import check_GNU_style_file

def main():
    parser = argparse.ArgumentParser(description='Check GNU coding style.')
    parser.add_argument('file', help = 'File with a patch')
    parser.add_argument('-f', '--format', default = 'stdio',
        help = 'Display format',
        choices = ['stdio', 'quickfix'])
    args = parser.parse_args()
    filename = args.file
    format = args.format

    if filename == '-':
        check_GNU_style_file(sys.stdin, format)
    else:
        with open(filename, newline='\n') as diff_file:
            check_GNU_style_file(diff_file, format)

main()
