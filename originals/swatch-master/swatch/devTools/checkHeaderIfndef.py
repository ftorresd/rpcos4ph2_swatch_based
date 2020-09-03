#!/usr/bin/env python
# coding=utf-8

'''
checkHeaderIfndef.py  -  Checks that #ifndef/#define/#endif in header files matches naming convention __PATH_TO_THISEHADER_HPP__ 
USAGE: checkHeaderIfndef.py <filenames>
'''

import re
import sys




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
    nr_headers_total = 0
    nr_headers_error = 0

    for filename in filenames:
        nr_headers_total
        print "-> FILE:", filename

        path_under_include = filename[filename.rfind("include")+7:]
        while path_under_include[0] == '/':
            path_under_include = path_under_include[1:]
        macro_name = "__" + path_under_include.upper().replace('/', '_').replace('.', '_') + "__"
        
        with open(filename, 'r') as f:
            line_nr = 0

            # Iterate over lines
            while True:
                line_nr += 1
                text = f.readline()

                if not text:
                   nr_headers_error += 1
                   print "ERROR,", filename+":"+str(line_nr), " - file doesn't contain any #ifndef line!"
                   break

                m = re.compile(r"\w*#ifndef.+").match(text)
                if m:
                    break

            if text:
                m = re.compile(r"^#ifndef " + macro_name + "$").match(text)
                if not m:
                   nr_headers_error += 1
                   print "ERROR,", filename+":"+str(line_nr), " - #ifndef line does not conform to convention; expecting", "'#ifndef "+macro_name+"'"
                else:
                    # Next line after ifndef should be the #define line
                    text = f.readline()
                    m = re.compile(r"^#define " + macro_name + "$").match(text)
                    if not m:
                       nr_headers_error += 1
                       print "ERROR,", filename+":"+str(line_nr), " - #define line does not conform to convention; expecting", "'#define "+macro_name+"'"


    print '\nSUMMARY:', len(filenames), 'files,', 'all good' if (nr_headers_error == 0) else (str(nr_headers_error)+' errors')
    if nr_headers_error > 0:
        sys.exit(1)
