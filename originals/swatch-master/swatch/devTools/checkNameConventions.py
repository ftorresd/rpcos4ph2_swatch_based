#!/usr/bin/env python
# coding=utf-8


'''
checkNameConventions.py - Finds names of classes, parameters, and variables that don't follow the SWATCH naming conventions (see docs/CONTRIBUTE.md)
N.B. Requires the cppcheckdata python module
USAGE: checkNameConventions.py <names of cppcheck dump files>
'''


import re
import sys

import cppcheckdata



class Inspector:
    exclPathPrefixes = ['/opt/xdaq/include', '/opt/cactus/include/boost']

    class _RuleViolation:
        def __init__(self, rule, file, lineNr, text):
            self.rule = rule
            self.file = file
            # Store line number as int so that it's easier to produce sorted output (e.g. "File.hpp:4" before "File.hpp:10") when displaying resultsis easier later on.
            self.lineNr = int(lineNr)
            self.text = text

        def __cmp__(self, other):
            return cmp((self.rule, self.file, self.lineNr, self.text), (other.rule, other.file, other.lineNr, other.text))

        # Trivial hash function implementation for storing rule violations in sets
        def __hash__(self):
            return hash(self.rule) ^ hash(self.file) ^ hash(self.lineNr) ^ hash (self.text)

        def __str__(self):
            return self.rule + " - " + self.file + ":" + str(self.lineNr) + "  " + self.text

    def __init__(self):
        self._failures = set()
        self._warnings = set()

    def addFailure(self, ruleName, fileName, lineNr, text):
        for d in Inspector.exclPathPrefixes:
            if fileName.startswith(d):
                return
        self._failures.add( Inspector._RuleViolation(ruleName, fileName, lineNr, text) )

    def addWarning(self, ruleName, fileName, lineNr, text):
        for d in Inspector.exclPathPrefixes:
            if fileName.startswith(d):
                return
        self._warnings.add( Inspector._RuleViolation(ruleName, fileName, lineNr, text) )

    def getNumberOfErrors(self):
        return len(self._failures)

    def getNumberOfWarnings(self):
        return len(self._warnings)

    def report(self):
        print self.__class__.__name__, ":", len(self._failures), "errors,", len(self._warnings), "warnings" 
        for x in sorted(self._failures):
            print '  ERROR:', x
        for x in sorted(self._warnings):
            print '  WARNING:', x 



class NamespaceInspector(Inspector):
    # Camel case (no numbers or underscores); first letter lowercase
    _regex = re.compile("^[a-z]+[a-zA-Z]+[0-9]*$")

    def __init__(self):
        Inspector.__init__(self)

    def check(self, data):
        for s in data.scopes:
            if (s.type == 'Namespace'):
                print 'NAMESPACE:', s.className
                if not NamespaceInspector._regex.match(s.className):
                    startTok = s.classStart
                    self.addFailure("Namespace", startTok.file, startTok.linenr, s.className)


class ClassNameInspector(Inspector):
    # Camel case (no numbers or underscores); first letter uppercase
    _normalRegex = re.compile("^[A-Z]+[a-zA-Z0-9]+$")
    _emptyClassRegex = re.compile("^[a-z]+[a-zA-Z0-9]+$")

    def __init__(self):
        Inspector.__init__(self)

    def check(self, data):
        for s in data.scopes:
            if (s.type == 'Class') or (s.type == 'Struct'):
                print 'CLASS:', s.className

                # Don't apply name regex to classes of standard STL-esque names
                if s.className in ('exception', 'iterator', 'const_iterator'):
                    print "  Ignoring class name, since standard"
                    continue

                # Class name regex is different for 'empty' classes that are used as tokens to 
                # ... change between different modes of operation in function calls (e.g. 
                # ... std::adopt_lock_t and other locking strategies)
                startTok = s.classStart
                if (s.classStart.next == s.classEnd) and (s.classStart.previous.previous.str in ["class","struct"]):
                    regex = ClassNameInspector._emptyClassRegex
                else:
                    regex = ClassNameInspector._normalRegex
                if not regex.match(s.className):
                    self.addFailure("ClassName", startTok.file, startTok.linenr, s.className)


class FunctionNameInspector(Inspector):
    # Camel case (no numbers or underscores); first letter lowercase
    _operatorRegex = re.compile("operator.+$")
    _normalRegex = re.compile("^[a-z]+[a-zA-Z0-9]+$")

    def __init__(self):
        Inspector.__init__(self)

    def check(self, data):
        for f in data.functions:
            print 'FUNCTION:', f.tokenDef.file+":"+str(f.tokenDef.linenr), f.name

            # Normal function name rules don't apply to constructors
            if f.tokenDef.scope.type in ('Class', 'Struct'):
                if f.name == f.tokenDef.scope.className:
                    print "  Ignoring CTOR:", f.name
                    continue

            if FunctionNameInspector._operatorRegex.match(f.name):
                print "  Ignoring operator"
            elif not FunctionNameInspector._normalRegex.match(f.name):
                tok = f.tokenDef
                if f.name == 'XEntry':
                    print "  name:", f.name
                    print "  argument:", f.argument
                    print "  tokenDef:", tok
                    print "    file:", tok.file
                    print "    linenr:", tok.linenr
                    print "    typeScope:", tok.typeScope
                    print "    scope:", tok.scope.type, tok.scope.className
                self.addFailure("FuncName", tok.file, tok.linenr, f.name)



def isPublic(aVar):
    scope = aVar.nameToken.scope
    assert(scope.type in ('Struct', 'Class'))

    # Members are public by default in struct, but private by default in classes
    isPub = True if (scope.type == 'Struct') else False

    tok = scope.classStart
    while tok is not aVar.nameToken:
        if tok.str == 'public:':
            isPub = True
        elif tok.str in ('protected:', 'private:'):
            isPub = False
        tok = tok.next
    return isPub

class VariableInspector(Inspector):
    # Private/protected member data: mCamelCase
    _privateMemberRegex = re.compile("^m[A-Z][a-zA-Z0-9]+$")
    # Public member data: camelCase (if first letter is m, k or s, then 2nd letter can't be uppercase - to distinguish from private/protected member variables)
    _publicMemberRegex = re.compile("^([mks][a-z0-9]|[a-jln-rt-z])[a-zA-Z0-9]+$")
    # Const static member data / global variables: kCamelCase / sCamelCase
    _staticMemberRegex = re.compile("^[ks][A-Z][a-zA-Z0-9]+$")
    # Parameters: aCamelCase
    _parameterRegex = re.compile("^a[A-Z][a-zA-Z0-9]+$")
    # Local variables: lCamelCase
    _localRegex = re.compile("^l[A-Z][a-zA-Z0-9]+$")


    def __init__(self):
        Inspector.__init__(self)

    def check(self, data):
         for v in data.variables:
             token = v.nameToken
             varName = token.str

             errorCondition = None
             warnCondition = None

             if v.isArgument:
                 errorCondition = ('PARAMETER', VariableInspector._parameterRegex)

             # Static vars
             elif v.isStatic:
                 errorCondition = ('STATIC', VariableInspector._staticMemberRegex)
             # Global vars
             elif token.scope.type in ('Namespace'):
                 errorCondition = ('GLOBAL', VariableInspector._staticMemberRegex)

             # Member vars
             elif token.scope.type in ('Struct', 'Class'):
                 if isPublic(v):
                     errorCondition = ("MEMBER (public)", VariableInspector._publicMemberRegex)
                 else:
                     errorCondition = ("MEMBER (priv/protected)", VariableInspector._privateMemberRegex)

             # Anything else must be a local var (i.e. defined within body of a function)
             else:
                 warnCondition = ("LOCAL", VariableInspector._localRegex)


             if (errorCondition is not None) and (not errorCondition[1].match(varName)):
                 print errorCondition[0], ': ', varName
                 self.addFailure(errorCondition[0], token.file, token.linenr, varName)
             if (warnCondition is not None) and (not warnCondition[1].match(varName)):
                 print warnCondition[0], ': ', varName
                 self.addWarning(warnCondition[0], token.file, token.linenr, varName)
                


if "-h" in sys.argv[1:]:
    print __doc__
    sys.exit(0)
elif "--help" in sys.argv[1:]:
    print __doc__
    sys.exit(0)
elif len(sys.argv) < 2:
    print "INCORRECT USAGE!"
    print __doc__
    sys.exit(1)


files = sys.argv[1:]
inspectors = [NamespaceInspector(), ClassNameInspector(), FunctionNameInspector(), VariableInspector()]

# 2) Process each file
for idx,filename in enumerate(files):
   print "\n--> Processing file '" + filename + "' (" + str(idx+1) + " of " + str(len(files)) + ")"
   data = cppcheckdata.parsedump(filename)
   for cfg in data.configurations:
       for i in inspectors:
           i.check(cfg)


# 3) Print report from each inspector
for i in inspectors:
    i.report()


# 4) Print brief summary report
print
print "SUMMARY:"
print " ", len(files), "files processed"

errorsFound = False
for i in inspectors:
    sys.stdout.write("  " + i.__class__.__name__ + ": ")

    if (i.getNumberOfErrors() > 0):
        sys.stdout.write(str(i.getNumberOfErrors()) + " errors, ")
        errorsFound = True
    else:
        sys.stdout.write("No errors, ")
    if (i.getNumberOfWarnings() > 0):
        sys.stdout.write(str(i.getNumberOfWarnings()) + " warnings")
    else:
        sys.stdout.write("no warnings")

    sys.stdout.write("\n")
    sys.stdout.flush()

if errorsFound:
    sys.exit(2)
