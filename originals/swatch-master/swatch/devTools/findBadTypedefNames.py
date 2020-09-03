#!/usr/bin/env python
# coding=utf-8

'''
findBadTypedefNames.py  - Finds typedefs whose names don't conform to CamelCase_t rule
USAGE: findBadTypedefNames.py <filenames>
'''

import re
import sys


# Typedef declaration regex: 2nd half is complicated to cope with
#  - If function pointer, name is 2nd subgroup
#  - Otherwise, if a "normal" type, name is 1st subgroup
TYPEDEF_REGEX = re.compile('typedef\s+[A-Za-z_][\w &\*<>\(\),:]*\s+(?:(\w+)|\(\s*\*(\w+)\s*\)\s*\([\w &\*<>,:]*\))\s*;')
# Name regex: CamelCase_t
TYPEDEF_NAME_REGEX = re.compile('^[A-Z][A-Za-z0-9]+_t$') # CamelCase_t



def filenameLinesToString(filename, lineTuple):
    startLine, endLine = lineTuple
    assert( isinstance(startLine, int) )
    assert( isinstance(endLine, int) )
    if startLine == endLine:
        return filename + ':' + str(startLine)
    else:
        return filename + ':' + str(startLine) + '-' + str(endLine)



if __name__ == "__main__":

    if len(sys.argv) <= 1:
        print "ERROR: Incorrect usage!"
        print __doc__
        sys.exit(1)
    elif "-h" in sys.argv[1:]:
        print __doc__
        sys.exit(0)
    elif "--help" in sys.argv[1:]:
        print __doc__
        sys.exit(0)

    filenames = sys.argv[1:]
    nr_typedefs_total = 0
    nr_typedefs_error = 0

    for filename in filenames:
        print "-> FILE:", filename
        with open(filename, 'r') as f:
            # Current line counter is a 2-tuple to cope with scenarios when typedef split over multiple lines
            #  - Element 0: start of range
            #  - Element 1: end of range (inclusive)
            current_lines = (0,0)

            # Iterate over lines
            while True:
                text = f.readline()
                current_lines = (current_lines[1]+1, current_lines[1]+1)

#                print "DEBUG ", filenameLinesToString(filename, current_lines), ' - starting afresh'

                # Break when reach end of file
                if not text:
                    break

                # Merge lines in case typedef is split over multiple lines 
                while True:
                    # Go to next line if doesn't contain exactly one typedef
                    nr_typedefs_current = text.count("typedef")
#                    print "DEBUG ", filenameLinesToString(filename, current_lines), ' -', nr_typedefs_current, 'typedefs'
                    if nr_typedefs_current == 0:
                        # If typedef doesn't start on this line, skip to next one ...
                        break
                    elif nr_typedefs_current > 1:
                        nr_typedefs_total += nr_typedefs_current
                        nr_typedefs_error += nr_typedefs_current
                        print " ERROR ", filenameLinesToString(filename, current_lines), " - multiple typedefs on same line"
                        break

                    # If get match with typedef regex, then check the typedef's name against TYPEDEF_NAME_REGEX
                    match = TYPEDEF_REGEX.search(text)
                    if match:
                        nr_typedefs_total += 1
                        # Typedef name could be subgroup 1 or 2 depending on if type is a function pointer
                        #   (other subgroup will be None since not part of match)
                        typedef_name = match.group(1) if (match.group(1) is not None) else match.group(2)
                        # If typedef name is const_iterator or iterator, then violation of normal naming rule is OK
                        if typedef_name in ('const_iterator', 'iterator'):
                            break
                        name_match = TYPEDEF_NAME_REGEX.match(typedef_name)
                        if not name_match:
                            nr_typedefs_error += 1
                            print " ERROR ", filenameLinesToString(filename,current_lines), ' - Invalid typedef name:', typedef_name
                        break
                    # Else if no part of string matches typedef regex, then add next line to string
                    else:
                        next_line = f.readline()
                        if not next_line:
                            nr_typedefs_total += 1
                            nr_typedefs_error += 1
                            print " INTERNAL ERROR ", filenameLinesToString(filename, current_lines), " - reached end of file trying to find this typedef's name"
                            break
                        text += next_line
                        current_lines = (current_lines[0], current_lines[1] + 1)


    print '\nSUMMARY:', len(filenames), 'files,', nr_typedefs_total, 'typedefs,', 'all good' if (nr_typedefs_error == 0) else (str(nr_typedefs_error)+' errors')
    if nr_typedefs_error > 0:
        sys.exit(1)
