#!/usr/bin/env python
############################################################################
# CONFIDENTIAL
#
# Copyright (c) 2015 - 2017 Qualcomm Technologies International, Ltd.
#
############################################################################
import sys
import getopt
import os.path
import re

def help_and_exit():
    print """
    FILE
      parse_symbols.py  -  This script parses symbol files output by kreadelf, and presents them in a more useful form.

    usage:
        parse_symbols  -l <linkscript> [-c] symbols file
        
        Examples:
                    parse_symbols <output_file_name>.tsym
                    parse_symbols -l linkscript_amber <output_file_name>.tsym

    Description:
        Parse a Kalimba symbols file generated by kreadelf, and beautify the output.
        
    Options:
        -c              :   Show 'chaff', i.e. boring symbols. Default is off.
        -l <linkscript> :   Use this linkscript for section definitions
        -w wordsize     :   Select 24/32 bit target
        """
    sys.exit(1)
    
class Arguments:
    # Class to read and hold the arguments of the script.
    def __init__(self, argv):
        # Assign the command-line options
        self.script_path  = argv[0]
        self.show_chaff = False
        self.arch4 = False
        
        try:
            opts, args = getopt.getopt(sys.argv[1:], "cl:w:h", ["help"])
        except getopt.GetoptError, err:
            help_and_exit()

        for o, a in opts:
            if o in ("-h", "--help"):
                help_and_exit()
            elif o == "-c":
                # show_chaff
                self.show_chaff = True
            elif (o == "-l"):
                # Set the link script
                self.linkscript = a
            elif (o == "-w"):
                # Set the target type
                if (a == "32"):
                    self.arch4 = True
            else:
                # Not a recognised option
                print( "ERROR: Unhandled option '%s'",o)
                help_and_exit()
                
        # Set the symbol file
        self.symbols_file = args[0]
        
        # Sanity check the parameters
        if (self.symbols_file == None):
            help_and_exit()



def read_file(file_name, stop_if_failure = True, split_to_lines = True):
    """ Reads the content of a file. For gaining some performance all the content is 
    read in one go. Additionally we can split the file into lines."""
    content = None
    try:
        with open(file_name) as file:
            content = file.read()
        if split_to_lines:
            return content.split("\n")
    except IOError as E:
        if stop_if_failure:
            sys.stderr.write("%s: unable to open %s\n" % (sys.argv[0], file_name))
            raise E
    
    return content

# Find out which region this address is in.
# Input parameters: address (integer), top_flags top two hex digits (integer)
# Returns: region or None if there isn't one defined.
def find_region(address, top_flags, overlays, regions):    
    # First, look in the top flags of the address (which are inserted by the compiler) 
    # to work out whether the address is in PM or DM.
    # If bit 31 is set, the symbol is code, rather than data.
    # If bit 24 is set, the symbol is in NVMEM (could be code or data).
    # So code in NVMEM starts "81" and code in PM RAM starts "80".
    is_code = 0
    in_nvmem = 0
    
    if (top_flags & 0x80):
        is_code = 1
    
    if (top_flags & 0x01):
        # Actually since the PM RAM addresses overlap with DM RAM (not PM ROM),
        # We don't need to do anything with this flag yet.
        in_nvmem = 1
    

    # Extract the overlays one at a time, sorted smallest first
    for key in sorted(overlays, key= lambda entry: overlays[entry].end - overlays[entry].start):
        if (overlays[key].type == "CODE" and not is_code): continue
        if (overlays[key].type == "DATA" and is_code): continue
        if (not in_nvmem): continue
        if ( address >= overlays[key].start and address <= overlays[key].end ) :
            return key
        
    
    
    # Now do the same for non-overlay regions
    for key in sorted(regions,key= lambda entry: regions[entry].end - regions[entry].start):
        if (regions[key].type == "CODE" and not is_code):continue
        if (regions[key].type == "DATA" and is_code):continue
        if ( address >= regions[key].start and address <= regions[key].end ) :
            return key
        
    
    return None


class Entry:
    #class to hold any data
    def __str__(self):
        s = ""
        for key in self.__dict__:
            s += str(key)
            try:
                value = hex(self.__dict__[key])
            except:
                value = str(self.__dict__[key])
            s+= ":" + value + ", "
        return s
            
                
    

if __name__ == "__main__":
    # read arguments
    arguments = Arguments(sys.argv)

    # Open the linkscript file and parse it.
    # We will want to use the region names when parsing the symbols file.
    link_data =read_file(arguments.linkscript, stop_if_failure = False)
    
    #                        | Region  Name |StartAddr|EndAddr| Type
    #                        |   group 1    |   g2    |  g3   |  g4 
    pattern_1 = re.compile(r"^region\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+);")
    
    #                        | Overlay Name  |Region |Bit width
    #                        |    group 1    |  g2   |   g3    
    pattern_2 = re.compile(r"^overlay\s+(\S+)\s+(\S+)\s+(\S+);")
    
    regions = {}
    overlays = {}
    
    for line in link_data:
        match = re.search(pattern_1, line)
        if match:
            entry = Entry()
            entry.name = match.group(1)
            entry.start = int(match.group(2),16)
            entry.end = int(match.group(3),16)
            entry.type = match.group(4)
            regions[entry.name] = entry
        else:
            
            match = re.search(pattern_2,line)
            if match:
                name = match.group(1)
                region = match.group(2)
                
                if region in regions:
                    overlays[name] = regions[region]
                else:
                    print "unsupported region:",region
                    sys.exit(1)

            
    symobol_data = read_file(arguments.symbols_file)
    #                    |  Num:| Value | Size | Type  | Bind  | Vis   |  Ndx  | [Name] |
    #                    |  g1  |  g2   |  g3  |  g4   |  g5   |  g6   |   g7  |  g8    |
    pattern = re.compile("(\d+):\s+(\S+)\s+(\d)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)?")
    
    constants = []# constants.
    stuff = {} # functions or const initialisers.
    chaff = {} # other stuff we probably don't care about.
    
    for line in symobol_data:
        match = re.search(pattern, line)
        if match:
            entry = Entry()
            entry.value = match.group(2)
            if arguments.arch4:
                entry.topflags = int(match.group(2)[0:2],16)
                if (entry.topflags & 0xf0) == 0x80:
                    entry.hexval = '0' + match.group(2)[1:]
                else:
                    entry.hexval = match.group(2)[0:]
            else:
                # Strip off the first two hex digits. We use them separately.
                entry.topflags = int(match.group(2)[0:2],16)
                entry.hexval = match.group(2)[2:]
            
            entry.realval = int(entry.hexval, 16)
            entry.size = match.group(3)
            entry.type = match.group(4)
            entry.bind = match.group(5)
            entry.vis = match.group(6)
            entry.ndx = match.group(7)
            entry.name = match.group(8)
        else:
            continue
        
        # Figure out which hash/array to put the entry in. All of the hashes are keyed on ndx;
        # ndx implies where in the memory map the thing lives, which is our best way
        # of figuring out what it's for.
        if ((entry.type == 'NOTYPE') and (entry.ndx == 'ABS')):
            # Constant definition
            constants.append(entry)
        elif ((entry.type == 'NOTYPE') and (entry.ndx == 'UND')):
            # These are usually pseudo-variables - put into the chaff
            if not entry.ndx in chaff:
                chaff[entry.ndx] = []
            chaff[entry.ndx].append(entry)
        elif ((entry.type == 'NOTYPE') and (entry.bind == 'GLOBAL')):
            # NOTYPE GLOBAL with a genuine Index seems quite rare. 
            # It often defines the __Base and __Limit symbols for a region.
            # Caution: the value for __Limit is actually <final address in the region> + 1. 
            if entry.name[-7:] == "__Limit":
                entry.region = find_region(entry.realval- 0x1, entry.topflags, overlays, regions)
            else:
                entry.region = find_region(entry.realval, entry.topflags, overlays, regions)
            
            if not entry.region:
                entry.region = entry.ndx
            
            if not entry.region in stuff:
                stuff[entry.region] = []
            stuff[entry.region].append(entry)
         
        elif (entry.type == 'SECTION'):
            # Don't do anything with these.
            pass
        elif (entry.type == 'FILE'):
            # Ignore these for now.
            pass    
        elif (entry.type == 'FUNC' or entry.type == 'OBJECT' or entry.type == 'NOTYPE'):
            # Build up a hash of arrays of entries.
            entry.region = find_region( entry.realval, entry.topflags, overlays, regions)
            if not entry.region:
                entry.region = entry.ndx
            
    
            # Check for 'boring' code labels, e.g. '.Lg77' or 'Lc_init_fault_1'
            if (entry.name[0:2] == ".L" or entry.name[0:3] == "Lc_"): 
                entry.boring = 1
            
            if not entry.region in stuff:
                stuff[entry.region] = []
            stuff[entry.region].append(entry)
        
        else:
            if not entry.region in chaff:
                chaff[entry.region] = []
            chaff[entry.region].append(entry)
        
 
    #
    # Now print our findings
    #
    print "\n*********************************************************************"
    print "        CONSTANTS "
    print "*********************************************************************"
    for entry in constants:
        print "  " + entry.name + ": " + str(entry.value)
    
    
    print "\n*********************************************************************"
    print "        SYMBOLS "
    print "*********************************************************************"
    for key in sorted(stuff):
        print "---------------------------------------------------------------------"
        print "        SECTION " + str(key) + ":" 
        print "---------------------------------------------------------------------"
        print "   Value  Size Type    Bind   Vis      Ndx Name "
        entries = stuff[key]
        for entry in sorted(entries, key = lambda entry: entry.realval) :
            line = "%-4s %5s %-7s %-6s %s   %2s %s"%(entry.hexval, entry.size, entry.type , entry.bind, entry.vis, entry.ndx, entry.name )   
            if arguments.show_chaff or not "boring" in entry.__dict__ :
                print line
            

    if arguments.show_chaff:
        print "\n*********************************************************************\n"
        print "        CHAFF \n"
        print "*********************************************************************\n"
        for key in sorted(chaff):
            print "---------------------------------------------------------------------"
            print "        SECTION " + str(key) + ":" 
            print "---------------------------------------------------------------------"
            print "   Value  Size Type    Bind   Vis      Ndx Name "
            entries = chaff[key]
            for entry in sorted(entries, key = lambda entry: entry.realval) :
                line = "%-4s %5s %-7s %-6s %s   %2s %s"%(entry.hexval, entry.size, entry.type , entry.bind, entry.vis, entry.ndx, entry.name )   
                print line
            
