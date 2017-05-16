#! /usr/bin/env python3.3
# -*- coding: utf-8 -*-
'''
Function:
  edit VC project file "xx.vcxproj"
  
  example:
  1. to edit the library project output files' name: EditVcProject.py -l ACE-vc110
  2. to edit the library project as static lib project: EditVcProject.py -s
  
              condition = definition.getAttribute("Condition");
            library = args.library;
            if condition.find("Win32") != -1:
                library = library + ".win32";
            else:
                library = library + ".x64";
                
            if condition.find("Release") != -1:
                library = library + ".r";
            else:
                library = library + ".d";
            
            library = "$(OutDir)\\" + library;
            definition.text = library;
'''
import sys
import time

from xml.etree import ElementTree as ET

def Main(argv):
    path = "TempDynamicConfig.xml";
    now = time.time();
    print("Start Time: ", time.strftime("%Y-%m-%d %X", time.localtime(now)));
          
    tree = ET.parse(path);
    refs = tree.find("ReferenceServiceList/ReferenceService");
    
    refsEvent = refs.find("Event");
    refsEvent.set("StartTime", time.strftime("%Y-%m-%d %X", time.localtime(now)));
    
    tmsses = tree.findall("TimeShiftedServiceList/TimeShiftedService");
    for i in range(0, len(tmsses)):     
        offset = 60 * i;
        tmss = tmsses[i];
        tmssEvents = tmss.findall("Event");
        for tmssEvent in tmssEvents:
            tmssEvent.set("StartTime", time.strftime("%Y-%m-%d %X", time.localtime(now + offset)));
            offset = offset + int(tmssEvent.get("Duration"));
            
    tree.write(path, encoding="utf-8", xml_declaration=True) 

if __name__ == '__main__':
    sys.exit(Main(sys.argv));
