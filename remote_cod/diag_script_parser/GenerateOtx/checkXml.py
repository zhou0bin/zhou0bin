from lxml import etree
import io
import os
import logging
xsdName='DiagnosticProcedureV6.xsd'
def CheckXML(dir):
    xmlschema_doc = etree.parse(xsdName)
    xmlschema = etree.XMLSchema(xmlschema_doc)
    #files = os.listdir(dir)
    errorList = []
    
    for file in os.listdir(dir):
        #check all xmls in this dir
        if not file.endswith('.xml'):
            continue
        #doc = etree.parse(dir + '//' + file)
        file_path = os.path.join(dir, file)  #Path stitching is compatible with both Windows and Linux   2025-06-09
        doc = etree.parse(file_path)
        ret = xmlschema.validate(doc)
        if not ret:
            logEntryList = xmlschema.error_log
            for oneEntry in logEntryList:
                errorItem = {}
                errorItem['line']      = oneEntry.line
                errorItem['column']    = oneEntry.column
                errorItem['message']   = oneEntry.message.replace('{http://autosar.org/schema/r4.0}','')
                errorItem['type']      = oneEntry.type
                errorItem['typeName']  = oneEntry.type_name
                errorItem['level']     = oneEntry.level
                errorItem['levelName'] = oneEntry.level_name
                errorItem['filename']  = oneEntry.filename.lstrip('file:/')
                errorList.append(errorItem)
    return errorList

def startChecking(dir):
    ##xmlError will print in xmlError.txt
    xmlLogger = 'xmlError.txt'
    fileInfo = io.open(xmlLogger, 'w+',encoding='UTF-8')
    fileInfo.seek(0)
    fileInfo.close()
    errorList = CheckXML(dir)
    logger = logging.getLogger(__name__)
    handler = logging.FileHandler(xmlLogger)
    handler.setLevel(level = logging.ERROR)
    formatter = logging.Formatter('%(message)s')
    handler.setFormatter(formatter)
    logger.addHandler(handler)
    
    if len(errorList) != 0:
        for oneErrorInfo in errorList:
            logError = 'file['+oneErrorInfo['filename']+']:line['+str(oneErrorInfo['line'])+']:'+oneErrorInfo['message']
            logger.error(logError)
        print("XML errors occur, see xmlError.txt")
        return False
    else:
        print("XML in " + dir + ' checked successfully!')
        return True
    
    ##check OK, continue to generate code
    ##check NOK, print error log
