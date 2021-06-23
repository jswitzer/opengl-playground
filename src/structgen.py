#!/usr/bin/env python
import json
import sys

"""
{"structs": [{
  "shortname": "ma64h",
  "longname": "ma64_header",
  "fields": [
    [ "uint32_t", "magic"],
    [ "uint32_t", "cputype"],
    [ "uint32_t", "cpusubtype"],
    [ "uint32_t", "filetype"],
    [ "uint32_t", "nloadcmds"],
    [ "uint32_t", "szloadcmds"],
    [ "uint32_t", "flags"],
    [ "uint32_t", "reserved"]
  ]
},
{
  "shortname": "ma64c",
  "longname": "ma64_command",
  "fields": [
    [ "uint32_t", "cmdtype"],
    [ "uint32_t", "cmdsize"]
  ]
}]}
"""
def generate(s, sdecl=False, fdecl=False, fdefs=False):
    if sdecl:
        print "typedef struct %s_s {" % s['longname']
        for x in s['fields']:
            print "    %s %s;" % (x[0], x[1])
        print "} %s_t;" % s['longname']
        print ""
    if fdecl:
        print "void %s_read(%s_t * s, char ** b, short le);" % (s['shortname'], s['longname'])
        print "void %s_write(%s_t * s, char ** b, short le);" % (s['shortname'], s['longname'])
        print "void %s_print(%s_t * s);" % (s['shortname'],s['longname'])
    if fdefs:
        print "void %s_read(%s_t * s, char ** b, short le) {" % (s['shortname'], s['longname'])
        for x in s['fields']:
            if x[1] in s.get('force_be', []):
                end = "0"
            elif x[1] in s.get('force_le',[]):
                end = "1"
            else:
                end = "le"
            if x[0] == "uint64_t":
                print "    s->%s = r64word(b, %s);" % (x[1], end)
            elif x[0] == "uint32_t":
                print "    s->%s = r32word(b, %s);" % (x[1], end)
            elif x[0] == "uint16_t":
                print "    s->%s = r16word(b, %s);" % (x[1], end)
        print "}"
        print ""
        print "void %s_write(%s_t * s, char ** b, short le) {" % (s['shortname'], s['longname'])
        for x in s['fields']:
            if x[1] in s.get('force_be',[]):
                end = "0"
            elif x[1] in s.get('force_le',[]):
                end = "1"
            else:
                end = "le"
            if x[0] == "uint64_t":
                print "    w64word(s->%s, b, %s);" % (x[1], end)
            elif x[0] == "uint32_t":
                print "    w32word(s->%s, b, %s);" % (x[1], end)
            elif x[0] == "uint16_t":
                print "    w16word(s->%s, b, %s);" % (x[1], end)
        print "}"
        print ""
        print "void %s_print(%s_t * s) {" % (s['shortname'],s['longname'])
        for x in s['fields']:
            if x[0] == "uint64_t":
                print "    printf(\"%s %s:\\t%%llu (%%x)\\n\", s->%s, s->%s);" % (s['shortname'], x[1], x[1], x[1])
            elif x[0] == "uint32_t":
                print "    printf(\"%s %s:\\t%%u (%%x)\\n\", s->%s, s->%s);" % (s['shortname'], x[1], x[1], x[1])
            elif x[0] == "uint16_t":
                print "    printf(\"%s %s:\\t%%u (%%x)\\n\", s->%s, s->%s);" % (s['shortname'], x[1], x[1], x[1])
        print "}"

def usage():
    print "Usage: %s <[filename]> <all|h|c>" % sys.argv[0]

def main():
    if len(sys.argv) < 2:
        return usage()
    with open(sys.argv[1], "r") as f:
        s = json.load(f)
        defname = "_" + sys.argv[1].upper().split(".")[0]
        if sys.argv[2] == "h":
            print "#ifndef %s" % defname
            print "#define %s" % defname
            print "#include <stdint.h>"
        if sys.argv[2] == "c":
            print "#include \"%s.h\"" % sys.argv[1].lower().split(".")[0]
            print "#include \"parse.h\""
            print "#include <stdio.h>"
        for x in s['structs']:
            if sys.argv[2] == "all":
                generate(x, sdecl=True, fdecl=True, fdefs=True)
            elif sys.argv[2] == "h":
                generate(x, sdecl=True, fdecl=True, fdefs=False)
            elif sys.argv[2] == "c":
                generate(x, sdecl=False, fdecl=False, fdefs=True)
            else:
                usage()
        if sys.argv[2] == "h":
            print "#endif"
if __name__ == "__main__":
    main()
