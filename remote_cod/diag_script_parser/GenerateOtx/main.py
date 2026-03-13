import os
import shutil
import string
import xsdToPython
from optparse import OptionParser
import generatedCode
import checkXml

#conmand in path of main.py for example:python main.py -d test -x DiagnosticProcedureV3.xsd
def optionParser():
    parser = OptionParser()
    parser.add_option("-f", "--forxml", action="store_true",
       dest="onexml",
       default=False,
       help="generate xml")
    parser.add_option("-d", "--dir", action="store_true",
       dest="dir",
       default=False,
       help="generate xmls in dir")
    parser.add_option("-x", "--xsd", action="store_true",
       dest="xsd",
       default=False,
       help="rebuilding xsd to xsdToPython.py")
    (options, args) = parser.parse_args()
    print(options, args)
    if options.xsd==True:
        for arg in args:
            if arg.endswith('.xsd'):
                updateXSD(arg)
    if options.onexml==True:
        for arg in args:
            if arg.endswith('.xml'):
                argPath = os.path.split(arg)
                generatedCode.parentDirector = argPath[0]
                if(checkXml.startChecking(generatedCode.parentDirector)):
                    otx_root = xsdToPython.parse(arg)   #Procedure
                    generatedCode.generateProcedure(otx_root)
    # if True:#debug code
    elif options.dir==True:
        # args = []
        # args.append("F:\\middle\\aCore_mw_SrcCode\\MiddlewareCode\\diag_script\\GenerateOtx\\new_xml_2021_03_30")
        for arg in args:
            if not arg.endswith('.xml') and not arg.endswith('.xsd'):
                generatedCode.parentDirector = arg
                fileName = ''
                files = os.listdir(generatedCode.parentDirector)
                if files.count('Main_Proc.xml') > 0:
                    fileName = 'Main_Proc.xml'
                elif files:
                    for file in files:
                        if(file.endswith('.xml')):
                            fileName = file
                            break
                if(fileName != ''):
                    print("first generated xml=", fileName)
                    if(checkXml.startChecking(generatedCode.parentDirector)):
                        fileName_path = os.path.join(generatedCode.parentDirector, fileName)  #Path stitching is compatible with both Windows and Linux   2025-06-09
                        otx_root = xsdToPython.parse(fileName_path)   #Procedure
                        generatedCode.generateProcedure(otx_root)

import time
import io
def updateXSD(xsdName):
    print("updateXSD")
    os.system("generateDS.py -f --silence -o xsdToPython.py " + xsdName)
    os.system("generateDS_Patch.py") #make patch to xsdToPython.py
    
    #rename xsdName in checkXml.py
    ##start
    checkXml.xsdName = xsdName
    fileInfo = io.open('checkXml.py', 'r+',encoding='UTF-8')
    for line in fileInfo.readlines():
        lineData = line.split()
        if lineData and 'xsdName=' in lineData[0]:
            oldName = lineData[0]
            newName = 'xsdName='+"'"+xsdName+"'"
            file = io.open('checkXml.py', 'r+',encoding='UTF-8')
            data = file.read()
            data = data.replace(oldName, newName)
            file.seek(0)
            file.write(data)
            file.close()
            break
    fileInfo.close()
    ##end

def main():
    dirPath = 'GeneratedCode'
    if(os.path.exists(dirPath)):
        shutil.rmtree(dirPath)
    os.mkdir(dirPath)
    optionParser()
    print("Done")

if ( __name__ == "__main__"):
    main()
