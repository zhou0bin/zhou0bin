# ！usr/bin/python
# -*- coding: utf-8 -*-

import copy
import json
import xmltodict
import os
import collections
printFlag = False  # False代表关闭打印ID和OID，True代表打开打印ID和OID
constPartList = ['CODED-CONST', 'TABLE-KEY', 'MATCHING-REQUEST-PARAM']


def getBaseInfo(baseVariant, odxDict):
    comparamRefs = baseVariant['COMPARAM-REFS']['COMPARAM-REF']

    flag = 1
    for comparamRef in comparamRefs:
        if comparamRef['@ID-REF'] == 'ISO_15765_2.CP_UniqueRespIdTable' and flag == 1:
            odxDict['Request_Address'] = int(comparamRef['COMPLEX-VALUE']['SIMPLE-VALUE'][2])
            odxDict['Response_Address'] = int(comparamRef['COMPLEX-VALUE']['SIMPLE-VALUE'][5])
            flag = 0

        if comparamRef['@ID-REF'] == 'ISO_15765_2.CP_As':
            odxDict['CP_As'] = int(comparamRef['SIMPLE-VALUE'])

        if comparamRef['@ID-REF'] == 'ISO_15765_2.CP_Ar':
            odxDict['CP_Ar'] = int(comparamRef['SIMPLE-VALUE'])

        if comparamRef['@ID-REF'] == 'ISO_15765_2.CP_Bs':
            odxDict['CP_Bs'] = int(comparamRef['SIMPLE-VALUE'])

        if comparamRef['@ID-REF'] == 'ISO_15765_2.CP_Br':
            odxDict['CP_Br'] = int(comparamRef['SIMPLE-VALUE'])

        if comparamRef['@ID-REF'] == 'ISO_15765_2.CP_Cs':
            odxDict['CP_Cs'] = int(comparamRef['SIMPLE-VALUE'])

        if comparamRef['@ID-REF'] == 'ISO_15765_2.CP_Cr':
            odxDict['CP_Cr'] = int(comparamRef['SIMPLE-VALUE'])


def getDtc(baseVariant, odxDict):
    dtcDops = baseVariant['DIAG-DATA-DICTIONARY-SPEC']
    if 'DTC-DOPS' in dtcDops:
        dtcDops = baseVariant['DIAG-DATA-DICTIONARY-SPEC']['DTC-DOPS']['DTC-DOP']
        dtcDopList = list()
        if isinstance(dtcDops, list):
            for dtpDopParam in dtcDops:
                dtpDop = dict()
                dtcDopDictParam = dict()
                dtcDopDictParam['ID'] = dtpDopParam['@ID']
                dtcDopDictParam['SHORT-NAME'] = dtpDopParam['SHORT-NAME']
                if 'DIAG-CODED-TYPE' in dtpDopParam:
                    diagCodedType = dict()
                    if '@BASE-TYPE-ENCODING' in dtpDopParam['DIAG-CODED-TYPE']:
                        diagCodedType['BASE-TYPE-ENCODING'] = dtpDopParam['DIAG-CODED-TYPE']['@BASE-TYPE-ENCODING']
                    if '@BASE-DATA-TYPE' in dtpDopParam['DIAG-CODED-TYPE']:
                        diagCodedType['BASE-DATA-TYPE'] = dtpDopParam['DIAG-CODED-TYPE']['@BASE-DATA-TYPE']
                    if '@TERMINATION' in dtpDopParam['DIAG-CODED-TYPE']:
                        diagCodedType['TERMINATION'] = dtpDopParam['DIAG-CODED-TYPE']['@TERMINATION']
                    if '@xsi:type' in dtpDopParam['DIAG-CODED-TYPE']:
                        diagCodedType['TYPE'] = dtpDopParam['DIAG-CODED-TYPE']['@xsi:type']
                    if 'MAX-LENGTH' in dtpDopParam['DIAG-CODED-TYPE']:
                        diagCodedType['MAX-LENGTH'] = dtpDopParam['DIAG-CODED-TYPE']['MAX-LENGTH']
                    if 'MIN-LENGTH' in dtpDopParam['DIAG-CODED-TYPE']:
                        diagCodedType['MIN-LENGTH'] = dtpDopParam['DIAG-CODED-TYPE']['MIN-LENGTH']
                    if 'BIT-LENGTH' in dtpDopParam['DIAG-CODED-TYPE']:
                        diagCodedType['BIT-LENGTH'] = dtpDopParam['DIAG-CODED-TYPE']['BIT-LENGTH']
                    dtcDopDictParam['DIAG-CODED-TYPE'] = diagCodedType
                if 'PHYSICAL-TYPE' in dtpDopParam:
                    physicalType = dict()
                    if '@BASE-DATA-TYPE' in dtpDopParam['PHYSICAL-TYPE']:
                        physicalType['BASE-DATA-TYPE'] = dtpDopParam['PHYSICAL-TYPE']['@BASE-DATA-TYPE']
                    if '@DISPLAY-RADIX' in dtpDopParam['PHYSICAL-TYPE']:
                        physicalType['DISPLAY-RADIX'] = dtpDopParam['PHYSICAL-TYPE']['@DISPLAY-RADIX']
                    dtcDopDictParam['PHYSICAL-TYPE'] = physicalType
                if 'COMPU-METHOD' in dtpDopParam:
                    dtcDopDictParam['COMPU-METHOD'] = dtpDopParam['COMPU-METHOD']['CATEGORY']
                dtcList = list()
                dtcs = baseVariant['DIAG-DATA-DICTIONARY-SPEC']['DTC-DOPS']['DTC-DOP']['DTCS']['DTC']
                for dtc in dtcs:
                    dtcObj = dict()
                    dtcObj['SHORT-NAME'] = dtc['SHORT-NAME']
                    dtcObj['TROUBLE-CODE'] = dtc['TROUBLE-CODE']
                    dtcObj['DISPLAY-TROUBLE-CODE'] = dtc['DISPLAY-TROUBLE-CODE']
                    text = dtc['TEXT']
                    dtcObj['TI'] = text['@TI']
                    dtcObj['TEXT'] = text['#text']
                    dtcList.append(dtcObj)
                dtcDopDictParam['DTCS'] = dtcList
                dtpDop['DTC-DOP'] = dtcDopDictParam
                dtcDopList.append(dtpDop)
        elif isinstance(dtcDops, dict):
            dtpDop = dict()
            dtcDopDictParam = dict()
            dtcDopDictParam['ID'] = dtcDops['@ID']
            dtcDopDictParam['SHORT-NAME'] = dtcDops['SHORT-NAME']
            if 'DIAG-CODED-TYPE' in dtcDops:
                diagCodedType = dict()
                if '@BASE-TYPE-ENCODING' in dtcDops['DIAG-CODED-TYPE']:
                    diagCodedType['BASE-TYPE-ENCODING'] = dtcDops['DIAG-CODED-TYPE']['@BASE-TYPE-ENCODING']
                if '@BASE-DATA-TYPE' in dtcDops['DIAG-CODED-TYPE']:
                    diagCodedType['BASE-DATA-TYPE'] = dtcDops['DIAG-CODED-TYPE']['@BASE-DATA-TYPE']
                if '@TERMINATION' in dtcDops['DIAG-CODED-TYPE']:
                    diagCodedType['TERMINATION'] = dtcDops['DIAG-CODED-TYPE']['@TERMINATION']
                if '@xsi:type' in dtcDops['DIAG-CODED-TYPE']:
                    diagCodedType['TYPE'] = dtcDops['DIAG-CODED-TYPE']['@xsi:type']
                if 'MAX-LENGTH' in dtcDops['DIAG-CODED-TYPE']:
                    diagCodedType['MAX-LENGTH'] = dtcDops['DIAG-CODED-TYPE']['MAX-LENGTH']
                if 'MIN-LENGTH' in dtcDops['DIAG-CODED-TYPE']:
                    diagCodedType['MIN-LENGTH'] = dtcDops['DIAG-CODED-TYPE']['MIN-LENGTH']
                if 'BIT-LENGTH' in dtcDops['DIAG-CODED-TYPE']:
                    diagCodedType['BIT-LENGTH'] = dtcDops['DIAG-CODED-TYPE']['BIT-LENGTH']
                dtcDopDictParam['DIAG-CODED-TYPE'] = diagCodedType
            if 'PHYSICAL-TYPE' in dtcDops:
                physicalType = dict()
                if '@BASE-DATA-TYPE' in dtcDops['PHYSICAL-TYPE']:
                    physicalType['BASE-DATA-TYPE'] = dtcDops['PHYSICAL-TYPE']['@BASE-DATA-TYPE']
                if '@DISPLAY-RADIX' in dtcDops['PHYSICAL-TYPE']:
                    physicalType['DISPLAY-RADIX'] = dtcDops['PHYSICAL-TYPE']['@DISPLAY-RADIX']
                dtcDopDictParam['PHYSICAL-TYPE'] = physicalType
            if 'COMPU-METHOD' in dtcDops:
                dtcDopDictParam['COMPU-METHOD'] = dtcDops['COMPU-METHOD']['CATEGORY']
            dtcList = list()
            dtcs = baseVariant['DIAG-DATA-DICTIONARY-SPEC']['DTC-DOPS']['DTC-DOP']['DTCS']['DTC']
            for dtc in dtcs:
                dtcObj = dict()
                dtcObj['SHORT-NAME'] = dtc['SHORT-NAME']
                dtcObj['TROUBLE-CODE'] = dtc['TROUBLE-CODE']
                dtcObj['DISPLAY-TROUBLE-CODE'] = dtc['DISPLAY-TROUBLE-CODE']
                text = dtc['TEXT']
                dtcObj['TI'] = text['@TI']
                dtcObj['TEXT'] = text['#text']
                dtcList.append(dtcObj)
            dtcDopDictParam['DTCS'] = dtcList
            dtpDop['DTC-DOP'] = dtcDopDictParam
            dtcDopList.append(dtpDop)

        odxDict['DTC-DOPS'] = dtcDopList


def getTable(baseVariant, tableDict):
    tables = baseVariant['DIAG-DATA-DICTIONARY-SPEC']['TABLES']['TABLE']
    if type(tables) == collections.OrderedDict:
        tableRows = tables['TABLE-ROW']
        if type(tableRows) == collections.OrderedDict :
            tableDict[tableRows['@ID']] = str(hex(int(tableRows['KEY'])))[2:].upper().zfill(4)
        else:
            for tableRow in tableRows:
                tableDict[tableRow['@ID']] = str(hex(int(tableRow['KEY'])))[2:].upper().zfill(4)
    else:
        for table in tables:
            tableRows = table['TABLE-ROW']
            if type(tableRows) == collections.OrderedDict :
                tableDict[tableRows['@ID']] = str(hex(int(tableRows['KEY'])))[2:].upper().zfill(4)
            else:
                for tableRow in tableRows:
                    tableDict[tableRow['@ID']] = str(hex(int(tableRow['KEY'])))[2:].upper().zfill(4)


def handleSnapshoot(baseVariant, param, paramDict, posResponseDict, paramDictList):
    endOfPduFields = baseVariant['DIAG-DATA-DICTIONARY-SPEC']['END-OF-PDU-FIELDS']['END-OF-PDU-FIELD']
    posResponseDict['START-REPEAT-BYTE-POSITION'] = param['BYTE-POSITION']  # add
    for endOfPduField in endOfPduFields:
        if 'DOP-REF' in param :
            if param['DOP-REF']['@ID-REF'] == endOfPduField['@ID']:
                basicStructureRef = endOfPduField['BASIC-STRUCTURE-REF']['@ID-REF']
                structures = baseVariant['DIAG-DATA-DICTIONARY-SPEC']['STRUCTURES']['STRUCTURE']
                for structure in structures:
                    if basicStructureRef == structure['@ID']:
                        structureParams = structure['PARAMS']['PARAM']
                        for structureParam in structureParams:
                            structureParamDict = dict()
                            if printFlag:
                                structureParamDict['OID'] = structureParam['@OID']
                            structureParamDict['TYPE'] = structureParam['@xsi:type']
                            structureParamDict['SHORT-NAME'] = structureParam['SHORT-NAME']
                            if 'BYTE-POSITION' in structureParam:
                                structureParamDict['BYTE-POSITION'] = str(int(structureParam['BYTE-POSITION']) + int(posResponseDict['START-REPEAT-BYTE-POSITION']))
                            if 'BYTE-LENGTH' in structureParam:
                                structureParamDict['BYTE-LENGTH'] = structureParam['BYTE-LENGTH']
                            if 'BIT-POSITION' in structureParam:
                                structureParamDict['BIT-POSITION'] = structureParam['BIT-POSITION']
                            if 'BIT-LENGTH' in structureParam:
                                structureParamDict['BIT-LENGTH'] = structureParam['BIT-LENGTH']
                            if 'PHYS-CONSTANT-VALUE' in structureParam:
                                structureParamDict['PHYS-CONSTANT-VALUE'] = structureParam['PHYS-CONSTANT-VALUE']
                            if 'DOP-REF' in structureParam:
                                structureParamDict['DOP-REF'] = structureParam['DOP-REF']['@ID-REF']
                            if structureParam['SHORT-NAME'] == 'SnapshotRecord':
                                # del structureParamDict['DOP-REF']
                                envDataDesc = baseVariant['DIAG-DATA-DICTIONARY-SPEC']['ENV-DATA-DESCS']['ENV-DATA-DESC']
                                if 'DOP-REF' in structureParam:
                                    if envDataDesc['@ID'] == structureParam['DOP-REF']['@ID-REF'] :
                                        envDataRef = envDataDesc['ENV-DATA-REFS']['ENV-DATA-REF']['@ID-REF']
                                        envData = baseVariant['DIAG-DATA-DICTIONARY-SPEC']['ENV-DATAS']['ENV-DATA']
                                        if envData['@ID'] == envDataRef:
                                            envDataParams = envData['PARAMS']['PARAM']
                                            for envDataParam in envDataParams:
                                                envDataParamDict = dict()
                                                if printFlag:
                                                    envDataParamDict['OID'] = envDataParam['@OID']
                                                envDataParamDict['TYPE'] = envDataParam['@xsi:type']
                                                envDataParamDict['SHORT-NAME'] = envDataParam['SHORT-NAME']
                                                if 'PHYSICAL-DEFAULT-VALUE' in envDataParam:
                                                    envDataParamDict['PHYSICAL-DEFAULT-VALUE'] = envDataParam['PHYSICAL-DEFAULT-VALUE']
                                                if 'BYTE-POSITION' in envDataParam:
                                                    envDataParamDict['BYTE-POSITION'] = str(int(envDataParam['BYTE-POSITION']) + int(structureParamDict['BYTE-POSITION']))
                                                if 'BYTE-LENGTH' in envDataParam:
                                                    envDataParamDict['BYTE-LENGTH'] = envDataParam['BYTE-LENGTH']
                                                if 'BIT-POSITION' in envDataParam:
                                                    envDataParamDict['BIT-POSITION'] = envDataParam['BIT-POSITION']
                                                if 'BIT-LENGTH' in envDataParam:
                                                    envDataParamDict['BIT-LENGTH'] = envDataParam['BIT-LENGTH']
                                                if 'DOP-REF' in envDataParam:
                                                    envDataParamDict['DOP-REF'] = envDataParam['DOP-REF']['@ID-REF']
                                                paramDictList.append(envDataParamDict)
                            else:
                                paramDictList.append(structureParamDict)

    del paramDict['TYPE']
    del paramDict['SHORT-NAME']
    del paramDict['BYTE-POSITION']


def handleDop(baseVariant, tableRefId, dopList):
    tables = baseVariant['DIAG-DATA-DICTIONARY-SPEC']['TABLES']['TABLE']
    structures = baseVariant['DIAG-DATA-DICTIONARY-SPEC']['STRUCTURES']['STRUCTURE']
    for table in tables:
        if tableRefId == table['@ID']:
            tableRows = table['TABLE-ROW']
            for tableRow in tableRows:
                if 'STRUCTURE-REF' in tableRow:
                    structureRef = tableRow['STRUCTURE-REF']['@ID-REF']
                    for structure in structures:
                        if structureRef == structure['@ID']:
                            params = structure['PARAMS']['PARAM']
                            if isinstance(params, list):
                                for param in params:
                                    if 'DOP-REF' in param:
                                        dopList.append(param['DOP-REF']['@ID-REF'])
                            elif isinstance(params, dict):
                                if 'DOP-REF' in params:
                                    dopList.append(params['DOP-REF']['@ID-REF'])


def getPosResponseDictList(baseVariant, posResponseDictList):
    posResponses = baseVariant['POS-RESPONSES']['POS-RESPONSE']  # list
    for posResponse in posResponses:
        posResponseDict = dict()
        posResponseDict['ID'] = posResponse['@ID']   # can not delete
        if printFlag:
            posResponseDict['OID'] = posResponse['@OID']
        posResponseDict['SHORT-NAME'] = posResponse['SHORT-NAME']

        params = posResponse['PARAMS']['PARAM']
        paramDictList = list()
        if isinstance(params, list):
            for param in params:
                paramDict = dict()
                if printFlag:
                    if '@ID' in param:
                        paramDict['ID'] = param['@ID']
                    paramDict['OID'] = param['@OID']
                paramDict['TYPE'] = param['@xsi:type']
                paramDict['SHORT-NAME'] = param['SHORT-NAME']
                if 'BYTE-POSITION' in param:
                    paramDict['BYTE-POSITION'] = param['BYTE-POSITION']
                if 'BYTE-LENGTH' in param:
                    paramDict['BYTE-LENGTH'] = param['BYTE-LENGTH']
                if 'BIT-POSITION' in param:
                    paramDict['BIT-POSITION'] = param['BIT-POSITION']
                if 'BIT-LENGTH' in param:
                    paramDict['BIT-LENGTH'] = param['BIT-LENGTH']
                if 'DIAG-CODED-TYPE' in param:
                    diagCodedType = dict()
                    if '@BASE-TYPE-ENCODING' in param['DIAG-CODED-TYPE']:
                        diagCodedType['BASE-TYPE-ENCODING'] = param['DIAG-CODED-TYPE']['@BASE-TYPE-ENCODING']
                    if '@BASE-DATA-TYPE' in param['DIAG-CODED-TYPE']:
                        diagCodedType['BASE-DATA-TYPE'] = param['DIAG-CODED-TYPE']['@BASE-DATA-TYPE']
                    if '@TERMINATION' in param['DIAG-CODED-TYPE']:
                        diagCodedType['TERMINATION'] = param['DIAG-CODED-TYPE']['@TERMINATION']
                    if '@xsi:type' in param['DIAG-CODED-TYPE']:
                        diagCodedType['TYPE'] = param['DIAG-CODED-TYPE']['@xsi:type']
                    if 'MAX-LENGTH' in param['DIAG-CODED-TYPE']:
                        diagCodedType['MAX-LENGTH'] = param['DIAG-CODED-TYPE']['MAX-LENGTH']
                    if 'MIN-LENGTH' in param['DIAG-CODED-TYPE']:
                        diagCodedType['MIN-LENGTH'] = param['DIAG-CODED-TYPE']['MIN-LENGTH']
                    if 'BIT-LENGTH' in param['DIAG-CODED-TYPE']:
                        diagCodedType['BIT-LENGTH'] = param['DIAG-CODED-TYPE']['BIT-LENGTH']
                    paramDict['DIAG-CODED-TYPE'] = diagCodedType

                if 'CODED-VALUE' in param:
                    paramDict['CODED-VALUE'] = param['CODED-VALUE']
                if 'CODED-VALUES' in param:
                    codeValues = param['CODED-VALUES']['CODED-VALUE']
                    codeValueList = list()
                    if isinstance(codeValues, list):
                        for codeValue in codeValues:
                            codeValueList.append(codeValue)
                            paramDict['CODED-VALUE'] = codeValueList
                    else:
                        codeValueList.append(codeValues)
                        paramDict['CODED-VALUE'] = codeValueList

                if 'REQUEST-BYTE-POS' in param:
                    paramDict['REQUEST-BYTE-POS'] = param['REQUEST-BYTE-POS']
                if 'DOP-REF' in param:
                    if paramDict['SHORT-NAME'] in ['ListOfDTC', 'ListOfExtendedData', 'DTCAndStatusRecord']:  # ListOfDTC and ListOfExtendedData and DTCAndStatusRecord
                        handleSnapshoot(baseVariant, param, paramDict, posResponseDict, paramDictList)
                    else:
                        paramDict['DOP-REF'] = param['DOP-REF']['@ID-REF']

                if 'TABLE-REF' in param:
                    paramDict['TABLE-REF'] = param['TABLE-REF']['@ID-REF']
                if 'TABLE-KEY-REF' in param:
                    paramDict['TABLE-KEY-REF'] = param['TABLE-KEY-REF']['@ID-REF']
                if 'TABLE-ROW-REF' in param:
                    paramDict['TABLE-ROW-REF'] = param['TABLE-ROW-REF']['@ID-REF']

                if len(paramDict) != 0:
                    paramDictList.append(paramDict)
        elif isinstance(params, dict):
            paramDict = dict()
            if printFlag:
                paramDict['OID'] = params['@OID']
            paramDict['TYPE'] = params['@xsi:type']
            paramDict['SHORT-NAME'] = params['SHORT-NAME']
            if 'BYTE-POSITION' in params:
                paramDict['BYTE-POSITION'] = params['BYTE-POSITION']
            if 'BYTE-LENGTH' in params:
                paramDict['BYTE-LENGTH'] = params['BYTE-LENGTH']
            if 'BIT-POSITION' in params:
                paramDict['BIT-POSITION'] = params['BIT-POSITION']
            if 'BIT-LENGTH' in params:
                paramDict['BIT-LENGTH'] = params['BIT-LENGTH']
            if 'DIAG-CODED-TYPE' in params:
                diagCodedType = dict()
                if '@BASE-TYPE-ENCODING' in params['DIAG-CODED-TYPE']:
                    diagCodedType['BASE-TYPE-ENCODING'] = params['DIAG-CODED-TYPE']['@BASE-TYPE-ENCODING']
                if '@BASE-DATA-TYPE' in params['DIAG-CODED-TYPE']:
                    diagCodedType['BASE-DATA-TYPE'] = params['DIAG-CODED-TYPE']['@BASE-DATA-TYPE']
                if '@TERMINATION' in params['DIAG-CODED-TYPE']:
                    diagCodedType['TERMINATION'] = params['DIAG-CODED-TYPE']['@TERMINATION']
                if '@xsi:type' in params['DIAG-CODED-TYPE']:
                    diagCodedType['TYPE'] = params['DIAG-CODED-TYPE']['@xsi:type']
                if 'MAX-LENGTH' in params['DIAG-CODED-TYPE']:
                    diagCodedType['MAX-LENGTH'] = params['DIAG-CODED-TYPE']['MAX-LENGTH']
                if 'MIN-LENGTH' in params['DIAG-CODED-TYPE']:
                    diagCodedType['MIN-LENGTH'] = params['DIAG-CODED-TYPE']['MIN-LENGTH']
                if 'BIT-LENGTH' in params['DIAG-CODED-TYPE']:
                    diagCodedType['BIT-LENGTH'] = params['DIAG-CODED-TYPE']['BIT-LENGTH']
                paramDict['DIAG-CODED-TYPE'] = diagCodedType

            if 'CODED-VALUE' in params:
                paramDict['CODED-VALUE'] = params['CODED-VALUE']
            if 'CODED-VALUES' in params:
                codeValues = params['CODED-VALUES']['CODED-VALUE']
                codeValueList = list()
                if isinstance(codeValues, list):
                    for codeValue in codeValues:
                        codeValueList.append(codeValue)
                        paramDict['CODED-VALUE'] = codeValueList
                else:
                    codeValueList.append(codeValues)
                    paramDict['CODED-VALUE'] = codeValueList

            if 'REQUEST-BYTE-POS' in params:
                paramDict['REQUEST-BYTE-POS'] = params['REQUEST-BYTE-POS']
            if 'DOP-REF' in params:
                if paramDict['SHORT-NAME'] in ['ListOfDTC', 'ListOfExtendedData', 'DTCAndStatusRecord']:  # ListOfDTC and ListOfExtendedData and DTCAndStatusRecord
                    handleSnapshoot(baseVariant, params, paramDict, posResponseDict, paramDictList)
                else:
                    paramDict['DOP-REF'] = params['DOP-REF']['@ID-REF']

            if 'TABLE-REF' in params:
                paramDict['TABLE-REF'] = params['TABLE-REF']['@ID-REF']
            if 'TABLE-KEY-REF' in params:
                paramDict['TABLE-KEY-REF'] = params['TABLE-KEY-REF']['@ID-REF']
            if 'TABLE-ROW-REF' in params:
                paramDict['TABLE-ROW-REF'] = params['TABLE-ROW-REF']['@ID-REF']

            if len(paramDict) != 0:
                paramDictList.append(paramDict)

        posResponseDict['PARAMS'] = paramDictList
        posResponseDictList.append(posResponseDict)


def getNegResponseDictList(baseVariant, negResponseDictList):
    negResponses = baseVariant['NEG-RESPONSES']['NEG-RESPONSE']  # list
    for negResponse in negResponses:
        negResponseDict = dict()
        negResponseDict['ID'] = negResponse['@ID']   # can not delete
        if printFlag:
            negResponseDict['OID'] = negResponse['@OID']
        negResponseDict['SHORT-NAME'] = negResponse['SHORT-NAME']

        params = negResponse['PARAMS']['PARAM']
        paramDictList = list()
        if isinstance(params, list):
            for param in params:
                if param['@xsi:type'] == 'NRC-CONST':
                    continue
                paramDict = dict()
                if printFlag:
                    paramDict['OID'] = param['@OID']
                paramDict['TYPE'] = param['@xsi:type']
                paramDict['SHORT-NAME'] = param['SHORT-NAME']
                if 'BYTE-POSITION' in param:
                    paramDict['BYTE-POSITION'] = param['BYTE-POSITION']
                if 'BYTE-LENGTH' in param:
                    paramDict['BYTE-LENGTH'] = param['BYTE-LENGTH']
                if 'BIT-POSITION' in param:
                    paramDict['BIT-POSITION'] = param['BIT-POSITION']
                if 'BIT-LENGTH' in param:
                    paramDict['BIT-LENGTH'] = param['BIT-LENGTH']
                if 'DIAG-CODED-TYPE' in param:
                    diagCodedType = dict()
                    if '@BASE-TYPE-ENCODING' in param['DIAG-CODED-TYPE']:
                        diagCodedType['BASE-TYPE-ENCODING'] = param['DIAG-CODED-TYPE']['@BASE-TYPE-ENCODING']
                    if '@BASE-DATA-TYPE' in param['DIAG-CODED-TYPE']:
                        diagCodedType['BASE-DATA-TYPE'] = param['DIAG-CODED-TYPE']['@BASE-DATA-TYPE']
                    if '@TERMINATION' in param['DIAG-CODED-TYPE']:
                        diagCodedType['TERMINATION'] = param['DIAG-CODED-TYPE']['@TERMINATION']
                    if '@xsi:type' in param['DIAG-CODED-TYPE']:
                        diagCodedType['TYPE'] = param['DIAG-CODED-TYPE']['@xsi:type']
                    if 'MAX-LENGTH' in param['DIAG-CODED-TYPE']:
                        diagCodedType['MAX-LENGTH'] = param['DIAG-CODED-TYPE']['MAX-LENGTH']
                    if 'MIN-LENGTH' in param['DIAG-CODED-TYPE']:
                        diagCodedType['MIN-LENGTH'] = param['DIAG-CODED-TYPE']['MIN-LENGTH']
                    if 'BIT-LENGTH' in param['DIAG-CODED-TYPE']:
                        diagCodedType['BIT-LENGTH'] = param['DIAG-CODED-TYPE']['BIT-LENGTH']
                    paramDict['DIAG-CODED-TYPE'] = diagCodedType

                if 'CODED-VALUE' in param:
                    paramDict['CODED-VALUE'] = param['CODED-VALUE']
                if 'CODED-VALUES' in param:
                    codeValues = param['CODED-VALUES']['CODED-VALUE']
                    codeValueList = list()
                    if isinstance(codeValues, list):
                        for codeValue in codeValues:
                            codeValueList.append(codeValue)
                            paramDict['CODED-VALUE'] = codeValueList
                    else:
                        codeValueList.append(codeValues)
                        paramDict['CODED-VALUE'] = codeValueList

                if 'DOP-REF' in param:
                    paramDict['DOP-REF'] = param['DOP-REF']['@ID-REF']

                if 'TABLE-REF' in param:
                    paramDict['TABLE-REF'] = param['TABLE-REF']['@ID-REF']
                if 'TABLE-KEY-REF' in param:
                    paramDict['TABLE-KEY-REF'] = param['TABLE-KEY-REF']['@ID-REF']
                if 'TABLE-ROW-REF' in param:
                    paramDict['TABLE-ROW-REF'] = param['TABLE-ROW-REF']['@ID-REF']

                paramDictList.append(paramDict)
        elif isinstance(params, dict):
            paramDict = dict()
            if printFlag:
             paramDict['OID'] = params['@OID']
            paramDict['TYPE'] = params['@xsi:type']
            paramDict['SHORT-NAME'] = params['SHORT-NAME']
            if 'BYTE-POSITION' in params:
                paramDict['BYTE-POSITION'] = params['BYTE-POSITION']
            if 'BYTE-LENGTH' in params:
                paramDict['BYTE-LENGTH'] = params['BYTE-LENGTH']
            if 'BIT-POSITION' in params:
                paramDict['BIT-POSITION'] = params['BIT-POSITION']
            if 'BIT-LENGTH' in params:
                paramDict['BIT-LENGTH'] = params['BIT-LENGTH']
            if 'DIAG-CODED-TYPE' in params:
                diagCodedType = dict()
                if '@BASE-TYPE-ENCODING' in params['DIAG-CODED-TYPE']:
                    diagCodedType['BASE-TYPE-ENCODING'] = params['DIAG-CODED-TYPE']['@BASE-TYPE-ENCODING']
                if '@BASE-DATA-TYPE' in params['DIAG-CODED-TYPE']:
                    diagCodedType['BASE-DATA-TYPE'] = params['DIAG-CODED-TYPE']['@BASE-DATA-TYPE']
                if '@TERMINATION' in params['DIAG-CODED-TYPE']:
                    diagCodedType['TERMINATION'] = params['DIAG-CODED-TYPE']['@TERMINATION']
                if '@xsi:type' in params['DIAG-CODED-TYPE']:
                    diagCodedType['TYPE'] = params['DIAG-CODED-TYPE']['@xsi:type']
                if 'MAX-LENGTH' in params['DIAG-CODED-TYPE']:
                    diagCodedType['MAX-LENGTH'] = params['DIAG-CODED-TYPE']['MAX-LENGTH']
                if 'MIN-LENGTH' in params['DIAG-CODED-TYPE']:
                    diagCodedType['MIN-LENGTH'] = params['DIAG-CODED-TYPE']['MIN-LENGTH']
                if 'BIT-LENGTH' in params['DIAG-CODED-TYPE']:
                    diagCodedType['BIT-LENGTH'] = params['DIAG-CODED-TYPE']['BIT-LENGTH']
                paramDict['DIAG-CODED-TYPE'] = diagCodedType

            if 'CODED-VALUE' in params:
                paramDict['CODED-VALUE'] = params['CODED-VALUE']
            if 'CODED-VALUES' in params:
                codeValues = params['CODED-VALUES']['CODED-VALUE']
                codeValueList = list()
                if isinstance(codeValues, list):
                    for codeValue in codeValues:
                        codeValueList.append(codeValue)
                        paramDict['CODED-VALUE'] = codeValueList
                else:
                    codeValueList.append(codeValues)
                    paramDict['CODED-VALUE'] = codeValueList

            if 'DOP-REF' in params:
                paramDict['DOP-REF'] = params['DOP-REF']['@ID-REF']

            if 'TABLE-REF' in params:
                paramDict['TABLE-REF'] = params['TABLE-REF']['@ID-REF']
            if 'TABLE-KEY-REF' in params:
                paramDict['TABLE-KEY-REF'] = params['TABLE-KEY-REF']['@ID-REF']
            if 'TABLE-ROW-REF' in params:
                paramDict['TABLE-ROW-REF'] = params['TABLE-ROW-REF']['@ID-REF']

            paramDictList.append(paramDict)

        negResponseDict['PARAMS'] = paramDictList
        negResponseDictList.append(negResponseDict)


def getRequestParam(baseVariant, tableDict, requestDictList, requestParamValueDict):
    requests = baseVariant['REQUESTS']['REQUEST']
    tables = baseVariant['DIAG-DATA-DICTIONARY-SPEC']['TABLES']['TABLE']
    for request in requests:
        requestDict = dict()
        requestDict['ID'] = request['@ID']   # can not delete
        if printFlag:
            requestDict['OID'] = request['@OID']
        requestDict['SHORT-NAME'] = request['SHORT-NAME']
        requestParams = request['PARAMS']['PARAM']
        requestParamDictList = list()
        requestParamValue = ""
        structureRef = ''
        for requestParam in requestParams:
            
            requestParamDict = dict()
            if printFlag:
                requestParamDict['OID'] = requestParam['@OID']
            requestParamDict['TYPE'] = requestParam['@xsi:type']
            requestParamDict['SHORT-NAME'] = requestParam['SHORT-NAME']
            if 'BYTE-POSITION' in requestParam:
                requestParamDict['BYTE-POSITION'] = requestParam['BYTE-POSITION']
            if 'BYTE-LENGTH' in requestParam:
                requestParamDict['BYTE-LENGTH'] = requestParam['BYTE-LENGTH']
            if 'BIT-POSITION' in requestParam:
                requestParamDict['BIT-POSITION'] = requestParam['BIT-POSITION']
            if 'BIT-LENGTH' in requestParam:
                requestParamDict['BIT-LENGTH'] = requestParam['BIT-LENGTH']
            if 'DIAG-CODED-TYPE' in requestParam:
                diagCodedType = dict()
                if '@BASE-TYPE-ENCODING' in requestParam['DIAG-CODED-TYPE']:
                    diagCodedType['BASE-TYPE-ENCODING'] = requestParam['DIAG-CODED-TYPE']['@BASE-TYPE-ENCODING']
                if '@BASE-DATA-TYPE' in requestParam['DIAG-CODED-TYPE']:
                    diagCodedType['BASE-DATA-TYPE'] = requestParam['DIAG-CODED-TYPE']['@BASE-DATA-TYPE']
                if '@TERMINATION' in requestParam['DIAG-CODED-TYPE']:
                    diagCodedType['TERMINATION'] = requestParam['DIAG-CODED-TYPE']['@TERMINATION']
                if '@xsi:type' in requestParam['DIAG-CODED-TYPE']:
                    diagCodedType['TYPE'] = requestParam['DIAG-CODED-TYPE']['@xsi:type']
                if 'MAX-LENGTH' in requestParam['DIAG-CODED-TYPE']:
                    diagCodedType['MAX-LENGTH'] = requestParam['DIAG-CODED-TYPE']['MAX-LENGTH']
                if 'MIN-LENGTH' in requestParam['DIAG-CODED-TYPE']:
                    diagCodedType['MIN-LENGTH'] = requestParam['DIAG-CODED-TYPE']['MIN-LENGTH']
                if 'BIT-LENGTH' in requestParam['DIAG-CODED-TYPE']:
                    diagCodedType['BIT-LENGTH'] = requestParam['DIAG-CODED-TYPE']['BIT-LENGTH']
                requestParamDict['DIAG-CODED-TYPE'] = diagCodedType

            if 'CODED-VALUE' in requestParam:
                requestParamDict['CODED-VALUE'] = requestParam['CODED-VALUE']
            if 'CODED-VALUES' in requestParam:
                codeValues = requestParam['CODED-VALUES']['CODED-VALUE']
                codeValueList = list()
                if isinstance(codeValues, list):
                    for codeValue in codeValues:
                        codeValueList.append(codeValue)
                        requestParamDict['CODED-VALUE'] = codeValueList
                else:
                    codeValueList.append(codeValues)
                    requestParamDict['CODED-VALUE'] = codeValueList

            if 'TABLE-REF' in requestParam:  # nothing to be done
                pass

            if 'TABLE-ROW-REF' in requestParam:
                if type(tables) == collections.OrderedDict:
                    tableRows = tables['TABLE-ROW']
                    if type(tableRows) == collections.OrderedDict:
                        if tableRows['@ID'] == requestParam['TABLE-ROW-REF']['@ID-REF']: 
                            requestParamDict['TABLE-ROW-REF'] = tableRows['KEY']
                            if 'STRUCTURE-REF' in tableRows:
                                structureRef = tableRows['STRUCTURE-REF']['@ID-REF']
                    else:
                        for tableRow in tableRows:
                            if tableRow['@ID'] == requestParam['TABLE-ROW-REF']['@ID-REF']:
                                requestParamDict['TABLE-ROW-REF'] = tableRow['KEY']
                                if 'STRUCTURE-REF' in tableRow:
                                    structureRef = tableRow['STRUCTURE-REF']['@ID-REF']
                else:
                    for table in tables:
                        tableRows = table['TABLE-ROW']
                        if type(tableRows) == collections.OrderedDict: 
                            if tableRows['@ID'] == requestParam['TABLE-ROW-REF']['@ID-REF']: 
                                requestParamDict['TABLE-ROW-REF'] = tableRows['KEY']
                                if 'STRUCTURE-REF' in tableRows:
                                    structureRef = tableRows['STRUCTURE-REF']['@ID-REF']
                                    break
                        else:
                            for tableRow in table['TABLE-ROW']: 
                                if tableRow['@ID'] == requestParam['TABLE-ROW-REF']['@ID-REF']:
                                    requestParamDict['TABLE-ROW-REF'] = tableRow['KEY']
                                    if 'STRUCTURE-REF' in tableRow:
                                        structureRef = tableRow['STRUCTURE-REF']['@ID-REF']
                                        break
                            

            tableKeyRefList = list()
            if 'TABLE-KEY-REF' in requestParam:
                structures = baseVariant['DIAG-DATA-DICTIONARY-SPEC']['STRUCTURES']['STRUCTURE']
                for structure in structures:
                    if structure['@ID'] == structureRef:
                        structureParams = structure['PARAMS']['PARAM']
                        if isinstance(structureParams, list):
                            for structureParam in structureParams:
                                dopRef = ''
                                if 'DOP-REF' in structureParam:
                                    dopRef = structureParam['DOP-REF']['@ID-REF']
                                    structureParamstructures = baseVariant['DIAG-DATA-DICTIONARY-SPEC']['STRUCTURES']['STRUCTURE']
                                    for structureParamstructure in structureParamstructures:
                                        if structureParamstructure['@ID'] == dopRef:
                                            param = structureParamstructure['PARAMS']['PARAM']
                                            if printFlag:
                                                requestParamDict['OID'] = param['@OID']
                                            if '@xsi:type' in param:
                                                requestParamDict['TYPE'] = param['@xsi:type']
                                            requestParamDict['SHORT-NAME'] = param['SHORT-NAME']
                                            if 'BYTE-POSITION' in param:
                                                requestParamDict['BYTE-POSITION'] = str(int(requestParamDict['BYTE-POSITION']) + int(param['BYTE-POSITION']))
                                            if 'BYTE-LENGTH' in param:
                                                requestParamDict['BYTE-LENGTH'] = param['BYTE-LENGTH']
                                            if 'BIT-POSITION' in param:
                                                requestParamDict['BIT-POSITION'] = param['BIT-POSITION']
                                            if 'BIT-LENGTH' in param:
                                                requestParamDict['BIT-LENGTH'] = param['BIT-LENGTH']
                                            if 'PHYS-CONSTANT-VALUE' in param:
                                                requestParamDict['PHYS-CONSTANT-VALUE'] = param['PHYS-CONSTANT-VALUE']
                                            requestParamDict['DOP-REF'] = param['DOP-REF']['@ID-REF']
                                if 'DOP-REF' not in requestParamDict:
                                    tableKeyRefDict = dict()
                                    if printFlag:
                                        tableKeyRefDict['OID'] = structureParam['@OID']
                                    if '@xsi:type' in structureParam:
                                        tableKeyRefDict['TYPE'] = structureParam['@xsi:type']
                                    tableKeyRefDict['SHORT-NAME'] = structureParam['SHORT-NAME']
                                    if 'BYTE-POSITION' in structureParam:
                                        tableKeyRefDict['BYTE-POSITION'] = str(int(requestParamDict['BYTE-POSITION']) + int(structureParam['BYTE-POSITION']))
                                    if 'BYTE-LENGTH' in structureParam:
                                        tableKeyRefDict['BYTE-LENGTH'] = structureParam['BYTE-LENGTH']
                                    if 'BIT-POSITION' in structureParam:
                                        tableKeyRefDict['BIT-POSITION'] = structureParam['BIT-POSITION']
                                    if 'BIT-LENGTH' in structureParam:
                                        tableKeyRefDict['BIT-LENGTH'] = structureParam['BIT-LENGTH']
                                    if 'PHYS-CONSTANT-VALUE' in structureParam:
                                        tableKeyRefDict['PHYS-CONSTANT-VALUE'] = structureParam['PHYS-CONSTANT-VALUE']
                                    if dopRef != '':
                                        tableKeyRefDict['DOP-REF'] = dopRef
                                    tableKeyRefList.append(tableKeyRefDict)

                        if isinstance(structureParams, dict):
                            if 'DOP-REF' in structureParams:
                                dopRef = structureParams['DOP-REF']['@ID-REF']
                                structureParamstructures = baseVariant['DIAG-DATA-DICTIONARY-SPEC']['STRUCTURES']['STRUCTURE']
                                for structureParamstructure in structureParamstructures:
                                    if structureParamstructure['@ID'] == dopRef:
                                        param = structureParamstructure['PARAMS']['PARAM']
                                        if type(param)==collections.OrderedDict:
                                            if printFlag:
                                                requestParamDict['OID'] = param['@OID']
                                            if '@xsi:type' in param:
                                                requestParamDict['TYPE'] = param['@xsi:type']
                                            requestParamDict['SHORT-NAME'] = param['SHORT-NAME']
                                            if 'BYTE-POSITION' in param:
                                                requestParamDict['BYTE-POSITION'] = str(int(param['BYTE-POSITION']) + int(requestParamDict['BYTE-POSITION']))
                                            if 'BYTE-LENGTH' in param:
                                                requestParamDict['BYTE-LENGTH'] = param['BYTE-LENGTH']
                                            if 'BIT-POSITION' in param:
                                                requestParamDict['BIT-POSITION'] = param['BIT-POSITION']
                                            if 'BIT-LENGTH' in param:
                                                requestParamDict['BIT-LENGTH'] = param['BIT-LENGTH']
                                            if 'PHYS-CONSTANT-VALUE' in param:
                                                requestParamDict['PHYS-CONSTANT-VALUE'] = param['PHYS-CONSTANT-VALUE']
                                            if 'DOP-REF' in param:
                                                requestParamDict['DOP-REF'] = param['DOP-REF']['@ID-REF']
                                        if type(param)==list:
                                            for par in param:
                                                if printFlag:
                                                    requestParamDict['OID'] = par['@OID']
                                                if '@xsi:type' in par:
                                                    requestParamDict['TYPE'] = par['@xsi:type']
                                                requestParamDict['SHORT-NAME'] = par['SHORT-NAME']
                                                if 'BYTE-POSITION' in par:
                                                    requestParamDict['BYTE-POSITION'] = str(int(par['BYTE-POSITION']) + int(requestParamDict['BYTE-POSITION']))
                                                if 'BYTE-LENGTH' in par:
                                                    requestParamDict['BYTE-LENGTH'] = par['BYTE-LENGTH']
                                                if 'BIT-POSITION' in par:
                                                    requestParamDict['BIT-POSITION'] = par['BIT-POSITION']
                                                if 'BIT-LENGTH' in par:
                                                    requestParamDict['BIT-LENGTH'] = par['BIT-LENGTH']
                                                if 'PHYS-CONSTANT-VALUE' in par:
                                                    requestParamDict['PHYS-CONSTANT-VALUE'] = par['PHYS-CONSTANT-VALUE']
                                                if 'DOP-REF' in par:
                                                    requestParamDict['DOP-REF'] = par['DOP-REF']['@ID-REF']

                                if 'DOP-REF' not in requestParamDict:
                                    if printFlag:
                                        requestParamDict['OID'] = structureParams['@OID']
                                    if '@xsi:type' in structureParams:
                                        requestParamDict['TYPE'] = structureParams['@xsi:type']
                                    requestParamDict['SHORT-NAME'] = structureParams['SHORT-NAME']
                                    if 'BYTE-POSITION' in structureParams:
                                        requestParamDict['BYTE-POSITION'] = str(int(structureParams['BYTE-POSITION']) + int(requestParamDict['BYTE-POSITION']))
                                    if 'BYTE-LENGTH' in structureParams:
                                        requestParamDict['BYTE-LENGTH'] = structureParams['BYTE-LENGTH']
                                    if 'BIT-POSITION' in structureParams:
                                        requestParamDict['BIT-POSITION'] = structureParams['BIT-POSITION']
                                    if 'BIT-LENGTH' in structureParams:
                                        requestParamDict['BIT-LENGTH'] = structureParams['BIT-LENGTH']
                                    if 'PHYS-CONSTANT-VALUE' in structureParams:
                                        requestParamDict['PHYS-CONSTANT-VALUE'] = structureParams['PHYS-CONSTANT-VALUE']
                                    requestParamDict['DOP-REF'] = dopRef

            if 'DOP-REF' in requestParam:
                requestParamDict['DOP-REF'] = requestParam['DOP-REF']['@ID-REF']

            requestParamDictList.append(requestParamDict)
            if len(tableKeyRefList) != 0:
                requestParamDictList.remove(requestParamDict)
                requestParamDictList.extend(tableKeyRefList)

            if requestParam['@xsi:type'] == 'CODED-CONST':
                requestParamValue += str(hex(int(requestParam['CODED-VALUE'])))[2:].upper().zfill(2)
            elif requestParam['@xsi:type'] == 'TABLE-KEY':
                requestParamValue += tableDict[requestParam['TABLE-ROW-REF']['@ID-REF']]

        requestParamValueDict[request['@ID']] = requestParamValue

        requestDict['PARAMS'] = requestParamDictList
        requestDictList.append(requestDict)


def hasTableKey(diagServicesObj):
    for param in diagServicesObj['POS-RESPONSE']['PARAMS']:
        if param['TYPE'] == 'TABLE-KEY':
            return True
    return False


def getDiagServices(baseVariant, requestDictList, requestParamValueDict, posResponseDictList, negResponseDictList,
                     diagServicesList):
    diagServices = baseVariant['DIAG-COMMS']['DIAG-SERVICE']
    for diagService in diagServices:
        diagServicesObj = dict()
        diagServicesObj['SHORT-NAME'] = diagService['SHORT-NAME']
        if printFlag:
            diagServicesObj['ID'] = diagService['@ID']
            diagServicesObj['OID'] = diagService['@OID']
        diagServicesObj['SEMANTIC'] = diagService['@SEMANTIC']
        diagServicesObj['SEND'] = ''
        diagServicesObj['POSRESPONSE'] = ''
        diagServicesObj['NEGRESPONSE'] = ''

        if printFlag:
            if 'PRE-CONDITION-STATE-REFS' in diagService:
                preConditionStateRefs = diagService['PRE-CONDITION-STATE-REFS']['PRE-CONDITION-STATE-REF']
                preConditionStateRefList = list()
                if isinstance(preConditionStateRefs, list):
                    for preConditionStateRef in preConditionStateRefs:
                        preConditionStateRefDict = dict()
                        preConditionStateRefDict['ID-REF'] = preConditionStateRef['@ID-REF']
                        if '@DOCREF' in preConditionStateRef:
                            preConditionStateRefDict['DOCREF'] = preConditionStateRef['@DOCREF']
                        if '@DOCTYPE' in preConditionStateRef:
                            preConditionStateRefDict['DOCTYPE'] = preConditionStateRef['@DOCTYPE']
                        preConditionStateRefList.append(preConditionStateRefDict)
                elif isinstance(preConditionStateRefs, dict):
                    preConditionStateRefDict = dict()
                    preConditionStateRefDict['ID-REF'] = preConditionStateRefs['@ID-REF']
                    if '@DOCREF' in preConditionStateRefs:
                        preConditionStateRefDict['DOCREF'] = preConditionStateRefs['@DOCREF']
                    if '@DOCTYPE' in preConditionStateRefs:
                        preConditionStateRefDict['DOCTYPE'] = preConditionStateRefs['@DOCTYPE']
                    preConditionStateRefList.append(preConditionStateRefDict)
                diagServicesObj['PRE-CONDITION-STATE-REFS'] = preConditionStateRefList

            if 'STATE-TRANSITION-REFS' in diagService:
                stateTransitionRefs = diagService['STATE-TRANSITION-REFS']['STATE-TRANSITION-REF']
                stateTransitionRefList = list()
                stateTransitionRefDict = dict()
                if isinstance(stateTransitionRefs, list):
                    for stateTransitionRef in stateTransitionRefs:
                        stateTransitionRefDict['ID-REF'] = stateTransitionRef['@ID-REF']
                        if '@DOCREF' in stateTransitionRef:
                            stateTransitionRefDict['DOCREF'] = stateTransitionRef['@DOCREF']
                        if '@DOCTYPE' in stateTransitionRef:
                            stateTransitionRefDict['DOCTYPE'] = stateTransitionRef['@DOCTYPE']
                        stateTransitionRefList.append(stateTransitionRefDict)
                elif isinstance(stateTransitionRefs, dict):
                    stateTransitionRefDict['ID-REF'] = stateTransitionRefs['@ID-REF']
                    if '@DOCREF' in stateTransitionRefs:
                        stateTransitionRefDict['DOCREF'] = stateTransitionRefs['@DOCREF']
                    if '@DOCTYPE' in stateTransitionRefs:
                        stateTransitionRefDict['DOCTYPE'] = stateTransitionRefs['@DOCTYPE']
                    stateTransitionRefList.append(stateTransitionRefDict)
                diagServicesObj['STATE-TRANSITION-REFS'] = stateTransitionRefList

        requestIdRef = diagService['REQUEST-REF']['@ID-REF']
        posResponseRef = diagService['POS-RESPONSE-REFS']['POS-RESPONSE-REF']['@ID-REF']
        negResponseRef = diagService['NEG-RESPONSE-REFS']['NEG-RESPONSE-REF']['@ID-REF']

        for requestDict in requestDictList:
            value = copy.deepcopy(requestDict)
            if str(requestIdRef) == str(value['ID']):
                if diagServicesObj['SEMANTIC'] in ['SESSION', 'RESET', 'SECURITY', 'TESTERPRESENT',
                                                   'CONTROL-DTC-SETTING']:
                    diagServicesObj['SEND'] = requestParamValueDict[value['ID']]
                    diagServicesObj['REQUEST'] = value
                    break
                elif diagServicesObj['SEMANTIC'] in ['IDENTIFICATION-READ', 'VARIANTCODINGREAD',
                                                     'MEASUREMENT-VALUES-READ', 'PARAMETRIZATION-READ',
                                                     'PARAMETRIZATION-WRITE', 'IDENTIFICATION-WRITE',
                                                     'VARIANTCODINGWRITE']:
                    did = requestParamValueDict[value['ID']][0:2] + requestParamValueDict[value['ID']][2:].zfill(4)
                    diagServicesObj['SEND'] = did
                    diagServicesObj['REQUEST'] = value
                    break
                elif diagServicesObj['SEMANTIC'] in ['IOCONTROL-START', 'IOCONTROL-STOP']:
                    did = requestParamValueDict[value['ID']][0:2] + \
                          requestParamValueDict[value['ID']][2:-2].zfill(4) + \
                          requestParamValueDict[value['ID']][-2:]
                    diagServicesObj['SEND'] = did
                    diagServicesObj['REQUEST'] = value
                    break
                # elif diagServicesObj['SEMANTIC'] == 'ROUTINE-START':
                elif diagServicesObj['SEMANTIC'] in ['ROUTINE-START', 'ROUTINE-STOP', 'ROUTINE-RESULTS']:
                    did = requestParamValueDict[value['ID']][0:4] + requestParamValueDict[value['ID']][4:].zfill(4)
                    diagServicesObj['SEND'] = did
                    diagServicesObj['REQUEST'] = value
                    break
                elif diagServicesObj['SEMANTIC'] == 'DEFAULT-FAULT-READ':
                    diagServicesObj['SEND'] = '1902'
                    diagServicesObj['REQUEST'] = value
                    break
                elif diagServicesObj['SEMANTIC'] == 'DEFAULT-FAULT-CLEAR':
                    diagServicesObj['SEND'] = '14'
                    diagServicesObj['REQUEST'] = value
                    break
                elif diagServicesObj['SEMANTIC'] == 'FAULT-EXTEND-READ':
                    diagServicesObj['SEND'] = '1906'
                    diagServicesObj['REQUEST'] = value
                    break
                elif diagServicesObj['SEMANTIC'] == 'DEFAULT-FAULT-COUNT':
                    diagServicesObj['SEND'] = '1901'
                    diagServicesObj['REQUEST'] = value
                    break
                elif diagServicesObj['SEMANTIC'] == 'FAULT-SUPP-READ':
                    diagServicesObj['SEND'] = '190A'
                    diagServicesObj['REQUEST'] = value
                    break
                elif diagServicesObj['SEMANTIC'] == 'DEFAULT-ENV-READ':
                    diagServicesObj['SEND'] = '1904'
                    diagServicesObj['REQUEST'] = value
                    break
                elif diagServicesObj['SEMANTIC'] == 'COMMUNICATION-CONTROL':
                    diagServicesObj['SEND'] = requestParamValueDict[value['ID']]
                    diagServicesObj['REQUEST'] = value
                    break

        tables = baseVariant['DIAG-DATA-DICTIONARY-SPEC']['TABLES']['TABLE']
        for posResponseDict in posResponseDictList:
            value = copy.deepcopy(posResponseDict)
            if str(posResponseRef) == str(value['ID']):
                params = value['PARAMS']
                structureRef = ''
                for param in params:
                    if 'TABLE-REF' in param:
                        if 'REQUEST' in diagServicesObj:
                            diagServicesObjRequestParams = diagServicesObj['REQUEST']['PARAMS']
                            for diagServicesObjRequestParam in diagServicesObjRequestParams:
                                if 'TABLE-ROW-REF' in diagServicesObjRequestParam:
                                    if type(tables) == collections.OrderedDict:
                                        if tables['@ID'] == param['TABLE-REF']:
                                            tableRows = tables['TABLE-ROW']
                                            if type(tableRows) == collections.OrderedDict:
                                                if diagServicesObjRequestParam['TABLE-ROW-REF'] == tableRows['KEY']:
                                                    if 'STRUCTURE-REF' in tableRows:
                                                        structureRef = tableRows['STRUCTURE-REF']['@ID-REF']
                                            else:
                                                for tableRow in tableRows:
                                                    if diagServicesObjRequestParam['TABLE-ROW-REF'] == tableRow['KEY']:
                                                        if 'STRUCTURE-REF' in tableRow:
                                                            structureRef = tableRow['STRUCTURE-REF']['@ID-REF']
                                    else:
                                        for table in tables:
                                            if table['@ID'] == param['TABLE-REF']:
                                                tableRows = table['TABLE-ROW']
                                                if type(tableRows) == collections.OrderedDict:
                                                    if diagServicesObjRequestParam['TABLE-ROW-REF'] == tableRows['KEY']:
                                                        if 'STRUCTURE-REF' in tableRows:
                                                            structureRef = tableRows['STRUCTURE-REF']['@ID-REF']
                                                else:
                                                    for tableRow in tableRows:
                                                        if diagServicesObjRequestParam['TABLE-ROW-REF'] == tableRow['KEY']:
                                                            if 'STRUCTURE-REF' in tableRow:
                                                                structureRef = tableRow['STRUCTURE-REF']['@ID-REF']


                                    param['TABLE-REF'] = diagServicesObjRequestParam['TABLE-ROW-REF']

                    elif 'TABLE-KEY-REF' in param:
                        tableKeyRefList = list()
                        structures = baseVariant['DIAG-DATA-DICTIONARY-SPEC']['STRUCTURES']['STRUCTURE']
                        for structure in structures:
                            if structure['@ID'] == structureRef:
                                structureParams = structure['PARAMS']['PARAM']
                                if isinstance(structureParams, list):
                                    for structureParam in structureParams:
                                        structureParamDict = dict()
                                        if printFlag:
                                            if 'ID' in structureParam:
                                                structureParamDict['ID'] = structureParam['@ID']
                                            structureParamDict['OID'] = structureParam['@OID']
                                        structureParamDict['TYPE'] = structureParam['@xsi:type']
                                        structureParamDict['SHORT-NAME'] = structureParam['SHORT-NAME']
                                        structureParamDict['BYTE-POSITION'] = str(int(structureParam['BYTE-POSITION']) + int(param['BYTE-POSITION']))
                                        if 'BYTE-LENGTH' in structureParam:
                                            structureParamDict['BYTE-LENGTH'] = structureParam['BYTE-LENGTH']
                                        if 'BIT-POSITION' in structureParam:
                                            structureParamDict['BIT-POSITION'] = structureParam['BIT-POSITION']
                                        if 'BIT-LENGTH' in structureParam:
                                            structureParamDict['BIT-LENGTH'] = structureParam['BIT-LENGTH']
                                        if 'PHYS-CONSTANT-VALUE' in structureParam:
                                            structureParamDict['PHYS-CONSTANT-VALUE'] = structureParam['PHYS-CONSTANT-VALUE']
                                        if 'DOP-REF' in structureParam:
                                            structureParamDict['DOP-REF'] = structureParam['DOP-REF']['@ID-REF']
                                        tableKeyRefList.append(structureParamDict)

                                if isinstance(structureParams, dict):
                                    structureParamDict = dict()
                                    if printFlag:
                                        if 'ID' in structureParams:
                                            structureParamDict['ID'] = structureParams['@ID']
                                        structureParamDict['OID'] = structureParams['@OID']
                                    structureParamDict['TYPE'] = structureParams['@xsi:type']
                                    structureParamDict['SHORT-NAME'] = structureParams['SHORT-NAME']
                                    structureParamDict['BYTE-POSITION'] = str(int(structureParams['BYTE-POSITION']) + int(param['BYTE-POSITION']))
                                    if 'BYTE-LENGTH' in structureParams:
                                        structureParamDict['BYTE-LENGTH'] = structureParams['BYTE-LENGTH']
                                    if 'BIT-POSITION' in structureParams:
                                        structureParamDict['BIT-POSITION'] = structureParams['BIT-POSITION']
                                    if 'BIT-LENGTH' in structureParams:
                                        structureParamDict['BIT-LENGTH'] = structureParams['BIT-LENGTH']
                                    if 'PHYS-CONSTANT-VALUE' in structureParams:
                                        structureParamDict['PHYS-CONSTANT-VALUE'] = structureParams['PHYS-CONSTANT-VALUE']
                                    if 'DOP-REF' in structureParams:
                                        structureParamDict['DOP-REF'] = structureParams['DOP-REF']['@ID-REF']
                                    tableKeyRefList.append(structureParamDict)
                        params.remove(param)
                        params.extend(tableKeyRefList)

                    elif 'TABLE-ROW-REF' in param:
                        for table in tables:
                            tableRows = table['TABLE-ROW']
                            for tableRow in tableRows:
                                if tableRow['@ID'] == param['TABLE-ROW-REF']:
                                    param['TABLE-ROW-REF'] = tableRow['KEY']

                diagServicesObj['POS-RESPONSE'] = value

        for negResponseDict in negResponseDictList:
            value = copy.deepcopy(negResponseDict)
            if str(negResponseRef) == str(value['ID']):
                params = value['PARAMS']
                structureRef = ''
                for param in params:
                    if 'TABLE-REF' in param:
                        for table in tables:
                            if table['@ID'] == param['TABLE-REF']:
                                tableRows = table['TABLE-ROW']
                                for tableRow in tableRows:
                                    for request in baseVariant['REQUESTS']['REQUEST']:
                                        if str(request['@ID']) == str(requestIdRef):
                                            for requestParam in request['PARAMS']['PARAM']:
                                                if 'TABLE-ROW-REF' in requestParam:
                                                    if requestParam['TABLE-ROW-REF']['@ID-REF'] == tableRow['@ID']:
                                                        param['TABLE-REF'] = tableRow['KEY']
                                                        if 'STRUCTURE-REF' in tableRow:
                                                            structureRef = tableRow['STRUCTURE-REF']['@ID-REF']

                    if 'TABLE-KEY-REF' in param:
                        tableKeyRefList = list()
                        structures = baseVariant['DIAG-DATA-DICTIONARY-SPEC']['STRUCTURES']['STRUCTURE']
                        for structure in structures:
                            if structure['@ID'] == structureRef:
                                structureParams = structure['PARAMS']['PARAM']
                                if isinstance(structureParams, list):
                                    for structureParam in structureParams:
                                        structureParamDict = dict()
                                        if printFlag:
                                            if 'ID' in structureParam:
                                                structureParamDict['ID'] = structureParam['@ID']
                                            structureParamDict['OID'] = structureParam['@OID']
                                        structureParamDict['TYPE'] = structureParam['@xsi:type']
                                        structureParamDict['SHORT-NAME'] = structureParam['SHORT-NAME']
                                        structureParamDict['BYTE-POSITION'] = str(int(structureParam['BYTE-POSITION']) + int(param['BYTE-POSITION']))
                                        if 'BYTE-LENGTH' in structureParam:
                                            structureParamDict['BYTE-LENGTH'] = structureParam['BYTE-LENGTH']
                                        if 'BIT-POSITION' in structureParam:
                                            structureParamDict['BIT-POSITION'] = structureParam['BIT-POSITION']
                                        if 'BIT-LENGTH' in structureParam:
                                            structureParamDict['BIT-LENGTH'] = structureParam['BIT-LENGTH']
                                        if 'PHYS-CONSTANT-VALUE' in structureParam:
                                            structureParamDict['PHYS-CONSTANT-VALUE'] = structureParam['PHYS-CONSTANT-VALUE']
                                        if 'DOP-REF' in structureParam:
                                            structureParamDict['DOP-REF'] = structureParam['DOP-REF']['@ID-REF']
                                        tableKeyRefList.append(structureParamDict)

                                if isinstance(structureParams, dict):
                                    structureParamDict = dict()
                                    if printFlag:
                                        if 'ID' in structureParams:
                                            structureParamDict['ID'] = structureParams['@ID']
                                        structureParamDict['OID'] = structureParams['@OID']
                                    structureParamDict['TYPE'] = structureParams['@xsi:type']
                                    structureParamDict['SHORT-NAME'] = structureParams['SHORT-NAME']
                                    structureParamDict['BYTE-POSITION'] = str(int(structureParams['BYTE-POSITION']) + int(param['BYTE-POSITION']))
                                    if 'BYTE-LENGTH' in structureParams:
                                        structureParamDict['BYTE-LENGTH'] = structureParams['BYTE-LENGTH']
                                    if 'BIT-POSITION' in structureParams:
                                        structureParamDict['BIT-POSITION'] = structureParams['BIT-POSITION']
                                    if 'BIT-LENGTH' in structureParams:
                                        structureParamDict['BIT-LENGTH'] = structureParams['BIT-LENGTH']
                                    if 'PHYS-CONSTANT-VALUE' in structureParams:
                                        structureParamDict['PHYS-CONSTANT-VALUE'] = structureParams['PHYS-CONSTANT-VALUE']
                                    if 'DOP-REF' in structureParams:
                                        structureParamDict['DOP-REF'] = structureParams['DOP-REF']['@ID-REF']
                                    tableKeyRefList.append(structureParamDict)
                        params.remove(param)
                        params.extend(tableKeyRefList)

                    if 'TABLE-ROW-REF' in param:
                        for table in tables:
                            tableRows = table['TABLE-ROW']
                            for tableRow in tableRows:
                                if tableRow['@ID'] == param['TABLE-ROW-REF']:
                                    param['TABLE-ROW-REF'] = tableRow['KEY']

                diagServicesObj['NEG-RESPONSE'] = value

        diagServicesList.append(diagServicesObj)

        if not printFlag:
            for diagServicesObj in diagServicesList:
                if 'ID' in diagServicesObj['REQUEST']:
                    del diagServicesObj['REQUEST']['ID']

                if 'ID' in diagServicesObj['POS-RESPONSE']:
                    del diagServicesObj['POS-RESPONSE']['ID']
                posResponse = ''
                for param in diagServicesObj['POS-RESPONSE']['PARAMS']:
                    if 'TYPE' in  param:
                        if param['TYPE'] in constPartList:
                            if param['TYPE'] == 'CODED-CONST':
                                posResponse += copy.deepcopy(hex(int(param['CODED-VALUE']))[2:].upper().zfill(2))
                            elif param['TYPE'] == 'TABLE-KEY':
                                if 'TABLE-ROW-REF' in param:
                                    posResponse += copy.deepcopy(hex(int(param['TABLE-ROW-REF']))[2:].upper().zfill(2))
                                elif 'TABLE-REF' in param:
                                    posResponse += copy.deepcopy(hex(int(param['TABLE-REF']))[2:].upper().zfill(4))
                            elif param['TYPE'] == 'MATCHING-REQUEST-PARAM':
                                if diagServicesObj['SEND'][:2] in ['22', '2E', '2F', '31'] and hasTableKey(diagServicesObj):
                                    pass
                                elif diagServicesObj['SEND'][:2] in ['10', '11', '27', '28']:
                                    requestParam = copy.deepcopy(diagServicesObj['REQUEST']['PARAMS'])[int(param['REQUEST-BYTE-POS'])]
                                    if requestParam['TYPE'] == 'CODED-CONST':
                                        posResponse += hex(int(requestParam['CODED-VALUE']))[2:].upper().zfill(2)
                                    elif requestParam['TYPE'] == 'TABLE-KEY':
                                        if 'TABLE-ROW-REF' in requestParam:
                                            posResponse += hex(int(requestParam['TABLE-ROW-REF']))[2:].upper().zfill(2)
                                        elif 'TABLE-REF' in requestParam:
                                            posResponse += hex(int(requestParam['TABLE-REF']))[2:].upper().zfill(2)
                                else:
                                    requestParam = copy.deepcopy(diagServicesObj['REQUEST']['PARAMS'])[int(param['REQUEST-BYTE-POS'])]
                                    if requestParam['TYPE'] == 'CODED-CONST':
                                        posResponse += copy.deepcopy(hex(int(requestParam['CODED-VALUE']))[2:].upper().zfill(4))
                                    elif requestParam['TYPE'] == 'TABLE-KEY':
                                        if 'TABLE-ROW-REF' in requestParam:
                                            posResponse += copy.deepcopy(hex(int(requestParam['TABLE-ROW-REF']))[2:].upper().zfill(2))
                                        elif 'TABLE-REF' in requestParam:
                                            posResponse += copy.deepcopy(hex(int(requestParam['TABLE-REF']))[2:].upper().zfill(2))
                diagServicesObj['POSRESPONSE'] = posResponse

                if 'ID' in diagServicesObj['NEG-RESPONSE']:
                    del diagServicesObj['NEG-RESPONSE']['ID']
                negResponse = ''
                for param in diagServicesObj['NEG-RESPONSE']['PARAMS']:
                    if param['TYPE'] in constPartList:
                        if param['TYPE'] == 'CODED-CONST':
                            negResponse += copy.deepcopy(hex(int(param['CODED-VALUE']))[2:].upper().zfill(2))
                        if param['TYPE'] == 'TABLE-KEY':
                            if 'TABLE-ROW-REF' in param:
                                negResponse += copy.deepcopy(hex(int(param['TABLE-ROW-REF']))[2:].upper().zfill(2))
                            elif 'TABLE-REF' in param:
                                negResponse += copy.deepcopy(hex(int(param['TABLE-REF']))[2:].upper().zfill(2))

                diagServicesObj['NEGRESPONSE'] = negResponse


def getUnits(baseVariant, output):
    unitSpec = baseVariant['DIAG-DATA-DICTIONARY-SPEC']
    if 'UNIT-SPEC' in unitSpec:
        unitSpec = baseVariant['DIAG-DATA-DICTIONARY-SPEC']['UNIT-SPEC']
        unit = unitSpec['UNITS']['UNIT']
        if isinstance(unit, list):
            for val in unit:
                output[val['@ID']] = val['DISPLAY-NAME']
        if isinstance(unit, dict):
            output[unit['@ID']] = unit['DISPLAY-NAME']


def getDataObjectProp(baseVariant, dataObjectPropDictList):
    dataObjectProps = baseVariant['DIAG-DATA-DICTIONARY-SPEC']['DATA-OBJECT-PROPS']['DATA-OBJECT-PROP']  # list
    for dataObjectProp in dataObjectProps:
        dataObjectPropDict = dict()
        dataObjectPropDict['ID'] = dataObjectProp['@ID']
        if printFlag:
            dataObjectPropDict['OID'] = dataObjectProp['@OID']
        dataObjectPropDict['SHORT-NAME'] = dataObjectProp['SHORT-NAME']

        if printFlag:
            if '@BASE-TYPE-ENCODING' in dataObjectProp['DIAG-CODED-TYPE']:
                dataObjectPropDict['BASE-TYPE-ENCODING'] = dataObjectProp['DIAG-CODED-TYPE']['@BASE-TYPE-ENCODING']
            if '@BASE-DATA-TYPE' in dataObjectProp['DIAG-CODED-TYPE']:
                dataObjectPropDict['BASE-DATA-TYPE'] = dataObjectProp['DIAG-CODED-TYPE']['@BASE-DATA-TYPE']
            if '@xsi:type' in dataObjectProp['DIAG-CODED-TYPE']:
                dataObjectPropDict['TYPE'] = dataObjectProp['DIAG-CODED-TYPE']['@xsi:type']
        else:
            diagCodedType = dict()
            if '@BASE-TYPE-ENCODING' in dataObjectProp['DIAG-CODED-TYPE']:
                diagCodedType['BASE-TYPE-ENCODING'] = dataObjectProp['DIAG-CODED-TYPE']['@BASE-TYPE-ENCODING']
            if '@BASE-DATA-TYPE' in dataObjectProp['DIAG-CODED-TYPE']:
                diagCodedType['BASE-DATA-TYPE'] = dataObjectProp['DIAG-CODED-TYPE']['@BASE-DATA-TYPE']
            if '@TERMINATION' in dataObjectProp['DIAG-CODED-TYPE']:
                diagCodedType['TERMINATION'] = dataObjectProp['DIAG-CODED-TYPE']['@TERMINATION']
            if '@xsi:type' in dataObjectProp['DIAG-CODED-TYPE']:
                diagCodedType['TYPE'] = dataObjectProp['DIAG-CODED-TYPE']['@xsi:type']
            if 'MAX-LENGTH' in dataObjectProp['DIAG-CODED-TYPE']:
                diagCodedType['MAX-LENGTH'] = dataObjectProp['DIAG-CODED-TYPE']['MAX-LENGTH']
            if 'MIN-LENGTH' in dataObjectProp['DIAG-CODED-TYPE']:
                diagCodedType['MIN-LENGTH'] = dataObjectProp['DIAG-CODED-TYPE']['MIN-LENGTH']
            if dataObjectProp['DIAG-CODED-TYPE']['@xsi:type'] == 'STANDARD-LENGTH-TYPE':
                diagCodedType['BIT-LENGTH'] = dataObjectProp['DIAG-CODED-TYPE']['BIT-LENGTH']
            elif dataObjectProp['DIAG-CODED-TYPE']['@xsi:type'] == 'MIN-MAX-LENGTH-TYPE':
                diagCodedType['BIT-LENGTH'] = 'DYNAMIC-LENGTH'
            dataObjectPropDict['DIAG-CODED-TYPE'] = diagCodedType
            if 'PHYSICAL-TYPE' in dataObjectProp:
                physicalType = dict()
                if '@BASE-DATA-TYPE' in dataObjectProp['PHYSICAL-TYPE']:
                    physicalType['BASE-DATA-TYPE'] = dataObjectProp['PHYSICAL-TYPE']['@BASE-DATA-TYPE']
                if '@DISPLAY-RADIX' in dataObjectProp['PHYSICAL-TYPE']:
                    physicalType['DISPLAY-RADIX'] = dataObjectProp['PHYSICAL-TYPE']['@DISPLAY-RADIX']
                dataObjectPropDict['PHYSICAL-TYPE'] = physicalType

        dataObjectPropDict['COMPU-METHOD'] = dataObjectProp['COMPU-METHOD']['CATEGORY']

        if dataObjectPropDict['COMPU-METHOD'] == 'IDENTICAL':
            if printFlag:
                funStr = "def identical_positive(x):\n"
                hexFunStr = "def identical_negative(y):\n"
                diagCodedType = dataObjectProp['DIAG-CODED-TYPE']
                baseTypeEncoding = ''
                baseDataType = ''
                if '@BASE-TYPE-ENCODING' in diagCodedType:
                    baseTypeEncoding = diagCodedType['@BASE-TYPE-ENCODING']
                if '@BASE-DATA-TYPE' in diagCodedType:
                    baseDataType = diagCodedType['@BASE-DATA-TYPE']

                if baseTypeEncoding == 'NONE' and baseDataType == 'A_BYTEFIELD':
                    funStr += '\treturn x\n'
                    hexFunStr += '\treturn y\n'
                    dataObjectPropDict['HEX-TO-PHYSIC'] = funStr
                    dataObjectPropDict['PHYSIC-TO-HEX'] = hexFunStr

                elif baseTypeEncoding == 'BCD-P' and baseDataType == 'A_UINT32':
                    funStr += '\ty = hex((x >> 4) * 10 + (x & 0x0f))[2:]\n'
                    funStr += '\treturn y\n'
                    hexFunStr += '\tx = (int(y / 10) << 4) + (y % 10)\n'
                    hexFunStr += '\treturn x\n'
                    dataObjectPropDict['HEX-TO-PHYSIC'] = funStr
                    dataObjectPropDict['PHYSIC-TO-HEX'] = hexFunStr

                elif baseTypeEncoding == 'NONE' and baseDataType in ['A_UINT32', 'A_INT32', 'A_FLOAT32', 'A_FLOAT64']:
                    funStr += '\treturn x\n'
                    hexFunStr += '\treturn y\n'
                    dataObjectPropDict['HEX-TO-PHYSIC'] = funStr
                    dataObjectPropDict['PHYSIC-TO-HEX'] = hexFunStr

                elif baseDataType == 'A_ASCIISTRING':
                    funStr += '\tlist_s = []\n'
                    funStr += '\tfor i in range(0, len(x), 2):\n'
                    funStr += '\t\tlist_s.append(chr(int(x[i:i+2],16)))\n'
                    funStr += '\treturn "".join(list_s)\n'

                    dataObjectPropDict['HEX-TO-PHYSIC'] = funStr

                    hexFunStr += '\tlist_h = []\n'
                    hexFunStr += '\tfor c in y:\n'
                    hexFunStr += '\t\tlist_h.append(str(hex(ord(c))[2:]))\n'
                    hexFunStr += '\treturn "".join(list_h)\n'
                    dataObjectPropDict['PHYSIC-TO-HEX'] = hexFunStr
            else:
                diagCodedType = dataObjectProp['DIAG-CODED-TYPE']
                baseTypeEncoding = ''
                baseDataType = ''
                if '@BASE-TYPE-ENCODING' in diagCodedType:
                    baseTypeEncoding = diagCodedType['@BASE-TYPE-ENCODING']
                if '@BASE-DATA-TYPE' in diagCodedType:
                    baseDataType = diagCodedType['@BASE-DATA-TYPE']

                if baseDataType == 'A_ASCIISTRING':
                    dataObjectPropDict['IS-ASCII'] = 'TRUE'
                elif baseTypeEncoding == 'BCD-P' and baseDataType == 'A_UINT32':
                    dataObjectPropDict['IS-BCD'] = 'TRUE'
                elif baseTypeEncoding == 'NONE' and baseDataType == 'A_BYTEFIELD':
                    dataObjectPropDict['IS-HEXDUMP'] = 'TRUE'

        elif dataObjectPropDict['COMPU-METHOD'] == 'TEXTTABLE':
            compuScales = dataObjectProp['COMPU-METHOD']['COMPU-INTERNAL-TO-PHYS']['COMPU-SCALES']['COMPU-SCALE']
            if printFlag:
                funStr = "def texttable_positive(x):\n"
                hexFunStr = "def texttable_negative(x):\n"
                if isinstance(compuScales, list):
                    for compuScale in compuScales:
                        if int(compuScale['LOWER-LIMIT']) == int(compuScale['UPPER-LIMIT']):
                            funStr += '\tif x == ' + compuScale['LOWER-LIMIT'] + ':\n'
                            insertValue = compuScale['COMPU-CONST']['VT']['#text']
                            funStr += '\t\treturn ' + '\"' + insertValue + '\"' + '\n'

                            hexFunStr += '\tif x == ' + '\"' + insertValue + '\"' + ':\n'
                            hexFunStr += '\t\treturn ' + compuScale['LOWER-LIMIT'] + '\n'
                        else:
                            funStr += '\tif ' + compuScale['LOWER-LIMIT'] + '<=x' + '<=' + compuScale['UPPER-LIMIT'] + ':\n'
                            insertValue = compuScale['COMPU-CONST']['VT']['#text']
                            funStr += '\t\treturn ' + '\"' + insertValue + '\"' + '\n'

                            hexFunStr += '\tif x == ' + '\"' + insertValue + '\"' + ':\n'
                            hexFunStr += '\t\treturn ' + compuScale['LOWER-LIMIT'] + '\n'

                elif isinstance(compuScales, dict):
                    funStr += '\tif x == ' + compuScales['LOWER-LIMIT'] + ':\n'
                    insertValue = compuScales['COMPU-CONST']['VT']['#text']
                    funStr += '\t\treturn ' + '\"' + insertValue + '\"' + '\n'

                    hexFunStr += '\tif x == ' + '\"' + insertValue + '\"' + ':\n'
                    hexFunStr += '\t\treturn ' + compuScales['LOWER-LIMIT'] + '\n'

                if 'INTERNAL-CONSTR' in dataObjectProp:
                    scaleConstrs = dataObjectProp['INTERNAL-CONSTR']['SCALE-CONSTRS']['SCALE-CONSTR']  # list
                    if isinstance(scaleConstrs, list):
                        for scaleConstr in scaleConstrs:
                            funStr += '\tif x in range(' + scaleConstr['LOWER-LIMIT'] + ',' + scaleConstr[
                                'UPPER-LIMIT'] + '):\n'
                            funStr += '\t\treturn ' + '\"' + scaleConstr['@VALIDITY'] + '\"' + '\n'

                            hexFunStr += '\tif x == ' + '\"' + scaleConstr['@VALIDITY'] + '\":\n'
                            hexFunStr += '\t\treturn ' + scaleConstr['LOWER-LIMIT'] + '\n'

                    elif isinstance(scaleConstrs, dict):
                        funStr += '\tif x in range(' + scaleConstrs['LOWER-LIMIT'] + ',' + scaleConstrs[
                            'UPPER-LIMIT'] + '):\n'
                        funStr += '\t\treturn ' + '\"' + scaleConstrs['@VALIDITY'] + '\"' + '\n'

                        hexFunStr += '\tif x == ' + '\"' + scaleConstrs['@VALIDITY'] + '\":\n'
                        hexFunStr += '\t\treturn ' + scaleConstrs['LOWER-LIMIT'] + '\n'

                # test
                # print(funStr)
                # print(hexFunStr)

                dataObjectPropDict['HEX-TO-PHYSIC'] = funStr
                dataObjectPropDict['PHYSIC-TO-HEX'] = hexFunStr
            else:
                hexToPhysic = list()
                physicToHex = list()
                if isinstance(compuScales, list):
                    for compuScale in compuScales:
                        hexToPhysicDict = dict()
                        hexToPhysicDict['MIN'] = compuScale['LOWER-LIMIT']
                        hexToPhysicDict['MAX'] = compuScale['UPPER-LIMIT']
                        hexToPhysicDict['PHYSICAL'] = compuScale['COMPU-CONST']['VT']['#text']
                        hexToPhysic.append(hexToPhysicDict)

                        physicToHexDict = dict()
                        physicToHexDict['PHYSICAL'] = compuScale['COMPU-CONST']['VT']['#text']
                        physicToHexDict['HEX'] = compuScale['LOWER-LIMIT']
                        physicToHex.append(physicToHexDict)

                    if 'INTERNAL-CONSTR' in dataObjectProp:
                        scaleConstrs = dataObjectProp['INTERNAL-CONSTR']['SCALE-CONSTRS']['SCALE-CONSTR']  # list
                        if isinstance(scaleConstrs, list):
                            for scaleConstr in scaleConstrs:
                                hexToPhysicDict = dict()
                                hexToPhysicDict['MIN'] = scaleConstr['LOWER-LIMIT']
                                hexToPhysicDict['MAX'] = scaleConstr['UPPER-LIMIT']
                                hexToPhysicDict['PHYSICAL'] = scaleConstr['@VALIDITY']
                                hexToPhysic.append(hexToPhysicDict)

                                # physicToHexDict = dict()
                                # physicToHexDict['PHYSICAL'] = scaleConstr['@VALIDITY']
                                # physicToHexDict['HEX'] = scaleConstr['LOWER-LIMIT']
                                # physicToHex.append(physicToHexDict)

                        elif isinstance(scaleConstrs, dict):
                            hexToPhysicDict = dict()
                            hexToPhysicDict['MIN'] = scaleConstrs['LOWER-LIMIT']
                            hexToPhysicDict['MAX'] = scaleConstrs['UPPER-LIMIT']
                            hexToPhysicDict['PHYSICAL'] = scaleConstrs['@VALIDITY']
                            hexToPhysic.append(hexToPhysicDict)

                            # physicToHexDict = dict()
                            # physicToHexDict['PHYSICAL'] = scaleConstrs['@VALIDITY']
                            # physicToHexDict['HEX'] = scaleConstrs['LOWER-LIMIT']
                            # physicToHex.append(physicToHexDict)

                    dataObjectPropDict['HEX-TO-PHYSIC'] = hexToPhysic
                    dataObjectPropDict['PHYSIC-TO-HEX'] = physicToHex

                elif isinstance(compuScales, dict):
                    hexToPhysicDict = dict()
                    hexToPhysicDict['MIN'] = compuScales['LOWER-LIMIT']
                    hexToPhysicDict['MAX'] = compuScales['UPPER-LIMIT']
                    hexToPhysicDict['PHYSICAL'] = compuScales['COMPU-CONST']['VT']['#text']
                    hexToPhysic.append(hexToPhysicDict)

                    physicToHexDict = dict()
                    physicToHexDict['PHYSICAL'] = compuScales['COMPU-CONST']['VT']['#text']
                    physicToHexDict['HEX'] = compuScales['LOWER-LIMIT']
                    physicToHex.append(physicToHexDict)

                    if 'INTERNAL-CONSTR' in dataObjectProp:
                        scaleConstrs = dataObjectProp['INTERNAL-CONSTR']['SCALE-CONSTRS']['SCALE-CONSTR']  # list
                        if isinstance(scaleConstrs, list):
                            for scaleConstr in scaleConstrs:
                                hexToPhysicDict = dict()
                                hexToPhysicDict['MIN'] = scaleConstr['LOWER-LIMIT']
                                hexToPhysicDict['MAX'] = scaleConstr['UPPER-LIMIT']
                                hexToPhysicDict['PHYSICAL'] = scaleConstr['@VALIDITY']
                                hexToPhysic.append(hexToPhysicDict)

                                # physicToHexDict = dict()
                                # physicToHexDict['PHYSICAL'] = scaleConstr['@VALIDITY']
                                # physicToHexDict['HEX'] = scaleConstr['LOWER-LIMIT']
                                # physicToHex.append(physicToHexDict)

                        elif isinstance(scaleConstrs, dict):
                            hexToPhysicDict = dict()
                            hexToPhysicDict['MIN'] = scaleConstrs['LOWER-LIMIT']
                            hexToPhysicDict['MAX'] = scaleConstrs['UPPER-LIMIT']
                            hexToPhysicDict['PHYSICAL'] = scaleConstrs['@VALIDITY']
                            hexToPhysic.append(hexToPhysicDict)

                            # physicToHexDict = dict()
                            # physicToHexDict['PHYSICAL'] = scaleConstrs['@VALIDITY']
                            # physicToHexDict['HEX'] = scaleConstrs['LOWER-LIMIT']
                            # physicToHex.append(physicToHexDict)

                    dataObjectPropDict['HEX-TO-PHYSIC'] = hexToPhysic
                    dataObjectPropDict['PHYSIC-TO-HEX'] = physicToHex

        elif dataObjectPropDict['COMPU-METHOD'] == 'LINEAR':
            if printFlag:
                compuRationalCoeffs = \
                    dataObjectProp['COMPU-METHOD']['COMPU-INTERNAL-TO-PHYS']['COMPU-SCALES']['COMPU-SCALE'][
                        'COMPU-RATIONAL-COEFFS']  # dict
                funStr = "def linear_positive(x):\n"
                hexFunStr = "def linear_negative(x):\n"
                compuNumerator = compuRationalCoeffs['COMPU-NUMERATOR']['V']
                numeratorV1 = compuNumerator[0]
                numeratorV2 = compuNumerator[1]

                compuDenominator = '1'
                if 'COMPU-DENOMINATOR' in compuRationalCoeffs:
                    compuDenominator = compuRationalCoeffs['COMPU-DENOMINATOR']['V']

                if compuDenominator != 0:
                    funStr += '\t' + 'y = ' + '(' + numeratorV1 + '+' + numeratorV2 + '*x' + ')' + '/' + compuDenominator + '\n'
                    funStr += '\treturn y\n'
                else:
                    funStr += '\tThe denominator is zero\n'

                if numeratorV2 != 0:
                    hexFunStr += '\t' + 'y = (' + 'x*' + compuDenominator + '-' + numeratorV1 + ')' + '/' + numeratorV2 + '\n'
                    hexFunStr += '\treturn y\n'
                else:
                    hexFunStr += '\tThe denominator is zero\n'

                dataObjectPropDict['HEX-TO-PHYSIC'] = funStr
                dataObjectPropDict['PHYSIC-TO-HEX'] = hexFunStr

                if 'UNIT-REF' in dataObjectProp:
                    unit = dict()
                    getUnits(baseVariant, unit)
                    dataObjectPropDict['UNIT'] = unit[dataObjectProp['UNIT-REF']['@ID-REF']]
            else:
                compuScale = dataObjectProp['COMPU-METHOD']['COMPU-INTERNAL-TO-PHYS']['COMPU-SCALES']['COMPU-SCALE']
                if 'LOWER-LIMIT' in compuScale:
                    if '@INTERVAL-TYPE' in compuScale['LOWER-LIMIT']:
                        dataObjectPropDict['LOWER-LIMIT'] = compuScale['LOWER-LIMIT']['@INTERVAL-TYPE']
                    else:
                        dataObjectPropDict['LOWER-LIMIT'] = compuScale['LOWER-LIMIT']
                if 'UPPER-LIMIT' in compuScale:
                    if '@INTERVAL-TYPE' in compuScale['UPPER-LIMIT']:
                        dataObjectPropDict['UPPER-LIMIT'] = compuScale['UPPER-LIMIT']['@INTERVAL-TYPE']
                    else:
                        dataObjectPropDict['UPPER-LIMIT'] = compuScale['UPPER-LIMIT']

                compuRationalCoeffs = dataObjectProp['COMPU-METHOD']['COMPU-INTERNAL-TO-PHYS']['COMPU-SCALES']['COMPU-SCALE']['COMPU-RATIONAL-COEFFS']  # dict

                compuNumerator = compuRationalCoeffs['COMPU-NUMERATOR']['V']
                numeratorV1 = compuNumerator[0]
                numeratorV2 = compuNumerator[1]

                compuDenominator = '1'
                if 'COMPU-DENOMINATOR' in compuRationalCoeffs:
                    compuDenominator = compuRationalCoeffs['COMPU-DENOMINATOR']['V']

                dataObjectPropDict['HEX-TO-PHYSIC'] = numeratorV1 + ';' + numeratorV2 + ';' + compuDenominator
                # dataObjectPropDict['PHYSIC-TO-HEX'] = compuDenominator + ';' + numeratorV1 + ';' + numeratorV2
                # dataObjectPropDict['PHYSIC-TO-HEX'] = '-' + numeratorV1 + ';' + compuDenominator + ';' + numeratorV2
                if numeratorV1.find(".") != -1 :
                    st = str(float(numeratorV1)*(-1)) if float(numeratorV1)>0 else str(abs(float(numeratorV1)))
                else:
                    st = str(int(numeratorV1)*(-1)) if int(numeratorV1)>0 else str(abs(int(numeratorV1)))
                dataObjectPropDict['PHYSIC-TO-HEX'] = st + ';' + compuDenominator + ';' + numeratorV2
                if 'UNIT-REF' in dataObjectProp:
                    unit = dict()
                    getUnits(baseVariant, unit)
                    if len(unit)==0:
                        continue
                    dataObjectPropDict['UNIT'] = unit[dataObjectProp['UNIT-REF']['@ID-REF']]

                '''
                if 'INTERNAL-CONSTR' in dataObjectProp:
                    scaleConstrs = dataObjectProp['INTERNAL-CONSTR']['SCALE-CONSTRS']['SCALE-CONSTR']  # list
                    dataObjectPropDict[scaleConstrs['@VALIDITY'] + '-LOWER-LIMIT'] = scaleConstrs['LOWER-LIMIT']
                    dataObjectPropDict[scaleConstrs['@VALIDITY'] + '-UPPER-LIMIT'] = scaleConstrs['UPPER-LIMIT']
                '''

        elif dataObjectPropDict['COMPU-METHOD'] == 'SCALE-LINEAR':
            hexToPhysic = list()
            physicToHex = list()
            compuScales = dataObjectProp['COMPU-METHOD']['COMPU-INTERNAL-TO-PHYS']['COMPU-SCALES']['COMPU-SCALE']
            for compuScale in compuScales:
                hexToPhysicDict = dict()
                if 'LOWER-LIMIT' in compuScale:
                    if '#text' in compuScale['LOWER-LIMIT']:
                        hexToPhysicDict['MIN'] = compuScale['LOWER-LIMIT']['#text']
                    elif '@INTERVAL-TYPE' in compuScale['LOWER-LIMIT']:
                        hexToPhysicDict['MIN'] = compuScale['LOWER-LIMIT']['@INTERVAL-TYPE']
                    else:
                        hexToPhysicDict['MIN'] = compuScale['LOWER-LIMIT']
                if 'UPPER-LIMIT' in compuScale:
                    if '#text' in compuScale['UPPER-LIMIT']:
                        hexToPhysicDict['MAX'] = compuScale['UPPER-LIMIT']['#text']
                    elif '@INTERVAL-TYPE' in compuScale['UPPER-LIMIT']:
                        hexToPhysicDict['MAX'] = compuScale['UPPER-LIMIT']['@INTERVAL-TYPE']
                    else:
                        hexToPhysicDict['MAX'] = compuScale['UPPER-LIMIT']

                compuRationalCoeffs = compuScale['COMPU-RATIONAL-COEFFS']  # dict
                compuNumerator = compuRationalCoeffs['COMPU-NUMERATOR']['V']
                numeratorV1 = compuNumerator[0]
                numeratorV2 = compuNumerator[1]

                compuDenominator = '1'
                if 'COMPU-DENOMINATOR' in compuRationalCoeffs:
                    compuDenominator = compuRationalCoeffs['COMPU-DENOMINATOR']['V']

                hexToPhysicDict['FORMULA'] = numeratorV1 + ';' + numeratorV2 + ';' + compuDenominator
                hexToPhysic.append(hexToPhysicDict)

                physicToHexDict = dict()
                try:
                    physicToHexDict['MIN'] = str(int((int(numeratorV1)+int(numeratorV2)*int(hexToPhysicDict['MIN']))/int(compuDenominator)))
                except:
                    physicToHexDict['MIN'] = 'INFINITE'
                try:
                    physicToHexDict['MAX'] = str(int((int(numeratorV1)+int(numeratorV2)*int(hexToPhysicDict['MAX']))/int(compuDenominator)))
                except:
                    physicToHexDict['MAX'] = 'INFINITE'
                # physicToHexDict['FORMULA'] = compuDenominator + ';' + numeratorV1 + ';' + numeratorV2
                # physicToHexDict['FORMULA'] = '-' + numeratorV1 + ';' + compuDenominator + ';' + numeratorV2
                physicToHexDict['FORMULA'] = str(int(numeratorV1)*(-1)) if int(numeratorV1)<0 else str(abs(int(numeratorV1)))  + ';' + compuDenominator + ';' + numeratorV2
                physicToHex.append(physicToHexDict)

            dataObjectPropDict['HEX-TO-PHYSIC'] = hexToPhysic
            dataObjectPropDict['PHYSIC-TO-HEX'] = physicToHex

            if 'UNIT-REF' in dataObjectProp:
                unit = dict()
                getUnits(baseVariant, unit)
                dataObjectPropDict['UNIT'] = unit[dataObjectProp['UNIT-REF']['@ID-REF']]

        dataObjectPropDictList.append(dataObjectPropDict)


def readFglUds(fglJsonDict, odxJsonDict):
    odxJsonBaseVariant = odxJsonDict['ODX']['DIAG-LAYER-CONTAINER']['BASE-VARIANTS']['BASE-VARIANT']
    fglFunctionalGroup = fglJsonDict['ODX']['DIAG-LAYER-CONTAINER']['FUNCTIONAL-GROUPS']['FUNCTIONAL-GROUP']

    odxJsonBaseVariant['FUNCT-CLASSS'] = fglFunctionalGroup['FUNCT-CLASSS']

    fglDops = fglFunctionalGroup['DIAG-DATA-DICTIONARY-SPEC']['DATA-OBJECT-PROPS']['DATA-OBJECT-PROP']
    for fglDop in fglDops:
        odxJsonBaseVariant['DIAG-DATA-DICTIONARY-SPEC']['DATA-OBJECT-PROPS']['DATA-OBJECT-PROP'].append(fglDop)

    odxJsonUnits = odxJsonBaseVariant['DIAG-DATA-DICTIONARY-SPEC']
    if 'UNIT-SPEC' in odxJsonUnits:
        fglUnits = fglFunctionalGroup['DIAG-DATA-DICTIONARY-SPEC']['UNIT-SPEC']['UNITS']['UNIT']  # dict
        odxJsonUnits = odxJsonBaseVariant['DIAG-DATA-DICTIONARY-SPEC']['UNIT-SPEC']['UNITS']['UNIT']
        if isinstance(odxJsonUnits, list):
            odxJsonBaseVariant['DIAG-DATA-DICTIONARY-SPEC']['UNIT-SPEC']['UNITS']['UNIT'].append(fglUnits)
        elif isinstance(odxJsonUnits, dict):
            odxJsonUnitList = list()
            odxJsonUnitList.append(odxJsonUnits)
            odxJsonUnitList.append(fglUnits)
            odxJsonBaseVariant['DIAG-DATA-DICTIONARY-SPEC']['UNIT-SPEC']['UNITS']['UNIT'] = odxJsonUnitList

    fglDiagServices = fglFunctionalGroup['DIAG-COMMS']['DIAG-SERVICE']
    for fglDiagService in fglDiagServices:
        odxJsonBaseVariant['DIAG-COMMS']['DIAG-SERVICE'].append(fglDiagService)

    fglRequests = fglFunctionalGroup['REQUESTS']['REQUEST']
    for fglRequest in fglRequests:
        odxJsonBaseVariant['REQUESTS']['REQUEST'].append(fglRequest)

    fglPosResponses = fglFunctionalGroup['POS-RESPONSES']['POS-RESPONSE']
    for fglPosResponse in fglPosResponses:
        odxJsonBaseVariant['POS-RESPONSES']['POS-RESPONSE'].append(fglPosResponse)

    fglNegResponses = fglFunctionalGroup['NEG-RESPONSES']['NEG-RESPONSE']
    for fglNegResponse in fglNegResponses:
        odxJsonBaseVariant['NEG-RESPONSES']['NEG-RESPONSE'].append(fglNegResponse)

    fglStateCharts = fglFunctionalGroup['STATE-CHARTS']['STATE-CHART']
    for fglStateChart in fglStateCharts:
        odxJsonBaseVariant['STATE-CHARTS']['STATE-CHART'].append(fglStateChart)

    fglComparamRefs = fglFunctionalGroup['COMPARAM-REFS']['COMPARAM-REF']
    for fglComparamRef in fglComparamRefs:
        odxJsonBaseVariant['COMPARAM-REFS']['COMPARAM-REF'].append(fglComparamRef)

    parentRefs = list()
    parentRefs.append(odxJsonBaseVariant['PARENT-REFS']['PARENT-REF'])
    fglParentRefs = fglFunctionalGroup['PARENT-REFS']['PARENT-REF']
    for fglParentRef in fglParentRefs:
        parentRefs.append(fglParentRef)
    odxJsonBaseVariant['PARENT-REFS']['PARENT-REF'] = parentRefs


def getStateChart(baseVariant, output):
    stateCharts = baseVariant['STATE-CHARTS']['STATE-CHART']
    for stateChart in stateCharts:
        stateChartDict = dict()
        if printFlag:
            stateChartDict['ID'] = stateChart['@ID']
            stateChartDict['OID'] = stateChart['@OID']
        stateChartDict['SHORT-NAME'] = stateChart['SHORT-NAME']
        if 'STATE-TRANSITIONS' in stateChart:
            stateTransitions = stateChart['STATE-TRANSITIONS']['STATE-TRANSITION']
            stateTransitionList = list()
            for stateTransition in stateTransitions:
                stateTransitionDict = dict()
                if printFlag:
                    stateTransitionDict['ID'] = stateTransition['@ID']
                    stateTransitionDict['OID'] = stateTransition['@OID']
                stateTransitionDict['SHORT-NAME'] = stateTransition['SHORT-NAME']
                stateTransitionDict['SOURCE-NAME'] = stateTransition['SOURCE-SNREF']['@SHORT-NAME']
                stateTransitionDict['TARGET-NAME'] = stateTransition['TARGET-SNREF']['@SHORT-NAME']
                stateTransitionList.append(stateTransitionDict)
            stateChartDict['STATE-TRANSITIONS'] = stateTransitionList
        states = stateChart['STATES']['STATE']
        stateList = list()
        for state in states:
            stateDict = dict()
            if printFlag:
                stateDict['ID'] = state['@ID']
                stateDict['OID'] = state['@OID']
            stateDict['SHORT-NAME'] = state['SHORT-NAME']
            stateList.append(stateDict)
        stateChartDict['STATES'] = stateList
        if 'START-STATE-SNREF' in stateChart:
            stateChartDict['START-STATE-SNREF'] = stateChart['START-STATE-SNREF']['@SHORT-NAME']
        output.append(stateChartDict)
# lis = list()
# def loop(dic:dict):
#     for key in dic.keys():
#         if type(dic[key]) == collections.OrderedDict:
#             loop(dic[key])
#         if type(dic[key]) == list:
#             print(key)
#             if key not in lis:
#                 lis.append(key)
#             for ele in dic[key]:
#                 if type(ele) == collections.OrderedDict:
#                     loop(ele)
# repetlis = ('DTC', 'SDG', 'DATA-OBJECT-PROP', 'COMPU-SCALE', 'V'
# , 'SCALE-CONSTR', 'STRUCTURE', 'PARAM', 'END-OF-PDU-FIELD'
# , 'UNIT', 'TABLE', 'TABLE-ROW', 'DIAG-SERVICE', 'STATE-TRANSITION-REF'
# , 'PRE-CONDITION-STATE-REF', 'REQUEST', 'POS-RESPONSE', 'NEG-RESPONSE'
# , 'CODED-VALUE', 'STATE-CHART', 'STATE', 'COMPARAM-REF', 'SIMPLE-VALUE'
# , 'NOT-INHERITED-DIAG-COMM')


if __name__ == '__main__':
    odxPath = 'odx-d/'
    odxFiles = os.listdir(odxPath)
    for odxFile in odxFiles:
        if odxFile == 'FGL_UDS.odx-d':
            continue
        if not os.path.isdir(odxFile):
            odxXmlFile = open(odxPath + '/' + odxFile, mode='r', encoding='utf-8')
            odxXmlStr = odxXmlFile.read()
            odxJsonDict = xmltodict.parse(odxXmlStr)

            fglXmlFile = open(odxPath + '/' + 'FGL_UDS.odx-d', 'r', encoding='utf-8')
            fglXmlStr = fglXmlFile.read()
            fglJsonDict = xmltodict.parse(fglXmlStr)

            readFglUds(fglJsonDict, odxJsonDict)

            # test
            '''
            jsonStrTest = json.dumps(odxJsonDict, indent=2)
            jsonFileTest = 'A18M_VCU_V1.0_20210330.odx-d.json'
            with open(jsonFileTest, mode='w', encoding='utf-8') as fileObj:
                fileObj.write(jsonStrTest)
            '''

            odxDict = dict()

            diagLayerContainer = odxJsonDict['ODX']['DIAG-LAYER-CONTAINER']
            odxDict['Ecu_Name'] = diagLayerContainer['SHORT-NAME']
            baseVariant = diagLayerContainer['BASE-VARIANTS']['BASE-VARIANT']

            getBaseInfo(baseVariant, odxDict)

            getDtc(baseVariant, odxDict)

            tableDict = dict()
            getTable(baseVariant, tableDict)

            requestDictList = list()
            requestParamValueDict = dict()
            getRequestParam(baseVariant, tableDict, requestDictList, requestParamValueDict)

            posResponseDictList = list()
            getPosResponseDictList(baseVariant, posResponseDictList)

            negResponseDictList = list()
            getNegResponseDictList(baseVariant, negResponseDictList)

            diagServicesList = list()
            getDiagServices(baseVariant, requestDictList, requestParamValueDict, posResponseDictList, negResponseDictList,
                            diagServicesList)

            odxDict['Diag_Service'] = diagServicesList

            dataObjectPropDictList = list()
            getDataObjectProp(baseVariant, dataObjectPropDictList)

            odxDict['Data_Object_Props'] = dataObjectPropDictList

            if printFlag:
                stateChartList = list()
                getStateChart(baseVariant, stateChartList)
                odxDict['STATE-CHARTS'] = stateChartList

            jsonStr = json.dumps(odxDict, indent=2, ensure_ascii=False)
            jsonFilePath = odxPath + '/odx-d.json/'
            if not os.path.exists(jsonFilePath):
                os.makedirs(jsonFilePath)
            jsonFile = jsonFilePath + odxFile[:-6] + '.json'
            print(jsonFile)
            with open(jsonFile, mode='w', encoding='utf-8') as fileObj:
                if not printFlag:
                    odxDictCopy = copy.deepcopy(odxDict)
                    diagService = odxDictCopy['Diag_Service']
                    for diagServiceItem in diagService:
                        requestIndex = 0
                        while requestIndex < len(diagServiceItem['REQUEST']['PARAMS']):
                            if diagServiceItem['REQUEST']['PARAMS'][requestIndex]['TYPE'] in constPartList:
                                del diagServiceItem['REQUEST']['PARAMS'][requestIndex]
                            else:
                                requestIndex += 1
                        if len(diagServiceItem['REQUEST']['PARAMS']) == 0:
                            del diagServiceItem['REQUEST']['PARAMS']

                        posResponseIndex = 0
                        while posResponseIndex < len(diagServiceItem['POS-RESPONSE']['PARAMS']):
                            try:
                                if diagServiceItem['POS-RESPONSE']['PARAMS'][posResponseIndex]['TYPE'] in constPartList:
                                    del diagServiceItem['POS-RESPONSE']['PARAMS'][posResponseIndex]
                                else:
                                    posResponseIndex += 1
                            except:
                                posResponseIndex += 1
                                continue
                        if len(diagServiceItem['POS-RESPONSE']['PARAMS']) == 0:
                            del diagServiceItem['POS-RESPONSE']['PARAMS']

                        negResponseIndex = 0
                        while negResponseIndex < len(diagServiceItem['NEG-RESPONSE']['PARAMS']):
                            if diagServiceItem['NEG-RESPONSE']['PARAMS'][negResponseIndex]['TYPE'] in constPartList:
                                del diagServiceItem['NEG-RESPONSE']['PARAMS'][negResponseIndex]
                            else:
                                negResponseIndex += 1
                        if len(diagServiceItem['NEG-RESPONSE']['PARAMS']) == 0:
                            del diagServiceItem['NEG-RESPONSE']['PARAMS']

                    jsonStrCopy = json.dumps(odxDictCopy, indent=2, ensure_ascii=False)
                    fileObj.write(jsonStrCopy)
                else:
                    fileObj.write(jsonStr)



