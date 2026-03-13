import tkinter.messagebox as tkMessageBox
import os
import xsdToPython

callProcedureStepList = []
lasVariablesList = []
LocalVariableList = []
pythonFileName = ''
setValueDict = {}
parentDirector = ''

def getAnnotationFun(otx_root):
    generate_procedure_annotation_string = ("#Procedure.Title:" + otx_root.get_Title() + "\n"
          "#Procedure.GUID:" + otx_root.get_GUID() + "\n"
          "#Procedure.Creator:" + otx_root.get_Creator() + "\n")
    writeToFile(generate_procedure_annotation_string)

    generate_editHistory_head_string = "#Procedure. EditHistoryList" + "\n"
    writeToFile(generate_editHistory_head_string)
    edithistorylistnode = otx_root.get_EditHistoryList()
    generate_editHistory_string = ""
    for edithistorynode in edithistorylistnode.get_EditHistory():
        generate_editHistory_string = ("# "+ edithistorynode.get_Editor() + " " + str(edithistorynode.get_EditTime()) + " "  + edithistorynode.get_EditContent()  + " " + edithistorynode.get_Version() + "\n")
        writeToFile(generate_editHistory_string)

def getImportLib(otx_root):
    import_lib_os = "import " + "os" + "\n"
    writeToFile(import_lib_os)
    import_lib_signal = "import " + "signal" + "\n"
    writeToFile(import_lib_signal)
    import_lib_sys = "import " + "sys" + "\n"
    writeToFile(import_lib_sys)
    import_lib_dirname = "sys.path[0]=(os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))))" + "\n"
    writeToFile(import_lib_dirname)
    # import_lib_enum = "from " + "enum " + "import " + "Enum" + "\n"
    # writeToFile(import_lib_enum)
    # import_lib_libraryfun = "from " + "libs.appAPI.LibraryFun  " + "import " + "*" + "\n"
    # writeToFile(import_lib_libraryfun)
    import_lib_localvariable = "from " + "libs.appAPI.LocalVariable " + "import " + "*" + "\n"
    writeToFile(import_lib_localvariable)
    import_lib_loginfo = "from " + "libs.appAPI.log_info " + "import " + "*\n"
    writeToFile(import_lib_loginfo)
    channelimports = "from libs.communication.pdu_logical_link import PDUOpenChannel,PDUCloseChannel \n"
    writeToFile(channelimports)
    dealodxserverimports = "from libs.util.dserver.odx_system import CallOdxServiceByShortName ,LoadOdx,UnloadOdx \n"
    writeToFile(dealodxserverimports)
    #Import the corresponding file according to the function
    importFuncs = set()
    apifileimports = "from libs.appAPI."
    Steps = otx_root.Steps.Step
    for arg in Steps:
        steps = arg.Steps.Step
        for step in steps:
            importFuncs.add(step.FunctionName)
    for func in importFuncs:
        if func not in ['CallOdxServiceByShortName',"LoadOdx","UnloadOdx","PDUOpenChannel","PDUCloseChannel"]:
            writeToFile(apifileimports + func + " import " + "*" + "\n")
    
    
    # import_lib_goto = "from " + "goto " + "import " + "with_goto" + "\n"
    # import_lib_time = "import " + "time" + "\n"
    # import_lib_json = "import " + "json" + "\n"
    # import_lib_localvariable = "from " + "LocalVariable " + "import " + "*" + "\n"
    # import_lib_libraryfun = "from " + "libs.LibraryFun  " + "import " + "*" + "\n"
    
    # import_lib_sys = "import " + "sys" + "\n"
    # # import_lib_dirname = "sys.path[0]=(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))" + "\n"
    # import_lib_dirname = "sys.path[0]=(os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))))" + "\n"
    # import_lib_appAPI = "from " + "libs.appAPI.LibraryFun " + "import " + "*" + "\n"
    # import_lib_communication = "from " + "libs.communication.pdu_logical_link " + "import " + "PDUOpenChannel" + "," + "PDUCloseChannel " + "\n"
    # import_lib_util = "from " + "libs.util.DiagLog " + "import " + "PrintLogInfo " + "as " + "WriteLog" + "\n"
    # import_lib_dserver = "from " + "libs.util.dserver.odx_system " + "import " + "*" + "\n"
    
    # generate_importlib_string = (import_lib_os + import_lib_sys + import_lib_dirname + import_lib_enum  + 
    #                              import_lib_appAPI )    
    # writeToFile(generate_importlib_string)

def generatedStatBlock(otx_root):
    statblock_string = ""
    statblocknode = otx_root.get_StatBlock()
    if(statblocknode is not None):
        statblock_string = ("statblockobj = " + "StatBlock(" + str(int(statblocknode.get_ID())) + ","
                            + "\"" + statblocknode.get_EnglishTitle() + "\"" + "," + "\"" + statblocknode.get_ChineseTitle() + "\"" + ","
                            + str(statblocknode.get_PrintAtNOK()).capitalize() + "," + str(statblocknode.get_PrintAtOK()).capitalize() + ")\n")
    writeToFile(statblock_string)

def getLocalVariableFun(otx_root):
    localVariablesnode = otx_root.get_LocalVariables()
    for localVariable in localVariablesnode.get_LocalVariable():
        generate_localVariab_string = ""
        defaultvalue_string = ""
        value_string = ""
        if localVariable.get_VariableType() == "int":
            defaultvalue_string = localVariable.get_DefaultValue()
            defaultvalue_string = 0
        if localVariable.get_VariableType() == "float" or localVariable.get_VariableType() == "double":
            defaultvalue_string = localVariable.get_DefaultValue()
            defaultvalue_string = 0.0
        if localVariable.get_VariableType() == "string" or localVariable.get_VariableType() == "LAS":
            defaultvalue_string = localVariable.get_DefaultValue()
            defaultvalue_string = "\"" + "" + "\""
        if localVariable.get_VariableType() == "bool":
            defaultvalue_string = localVariable.get_DefaultValue()
            defaultvalue_string = False
        if localVariable.get_VariableType() == "VciResult" or localVariable.get_VariableType() == "JsonObj" or localVariable.get_VariableType() == "StatStep" or localVariable.get_VariableType() == "ecuDtObj":
            defaultvalue_string = localVariable.get_DefaultValue()
            defaultvalue_string = None
        if localVariable.get_BaseDataType() == "int" or localVariable.get_BaseDataType() == "float" or localVariable.get_BaseDataType() == "double":
            value_string = localVariable.get_Value()
        if localVariable.get_BaseDataType() == "bool":
            value_string = localVariable.get_Value().capitalize()
        if localVariable.get_BaseDataType() == "string" or localVariable.get_BaseDataType() == "object":
            # value_string = "\"" + localVariable.get_Value() + "\""
            if localVariable.get_Value() == "None":
               value_string = str(None)
            else:
               value_string = "\"" + localVariable.get_Value() + "\""
        generate_localVariab_string = ("\"" + localVariable.get_BaseDataType()  + "\"" + 
                                           "," + "\"" + localVariable.get_GUID() + "\"" + "," + value_string + 
                                           "," + "\"" + localVariable.get_VariableType() + "\""    #xsdV5 delete InOutType in LocalVariable
                                           "," + str(defaultvalue_string)) 
        if localVariable.get_VariableType() == "ecuDtObj":
            generate_localVariab_string = (localVariable.get_VariableName() + " = " + localVariable.get_VariableType() + "(" + 
                                           "\"" + localVariable.get_Name() + "\"" + "," + str(hex(int(localVariable.get_RequestID()))) + 
                                           "," + str(hex(int(localVariable.get_ResponseID()))) + "," +
                                           str(localVariable.get_FrameType()) + "," + generate_localVariab_string + ")" + "\n")
        elif localVariable.get_VariableType() == "LAS":
            generate_localVariab_string = (localVariable.get_VariableName() + " = " + localVariable.get_VariableType() + "(" + 
                                   "\"" + localVariable.get_LASExpression() + "\"" + "," + generate_localVariab_string + ")" + "\n")
            lasVariablesList.append(localVariable)
        elif localVariable.get_VariableType() == "StatStep":
            generate_localVariab_string = (localVariable.get_VariableName() + " = " + localVariable.get_VariableType() + "(" + 
                                        str(localVariable.get_ID()) + "," + "\"" + localVariable.get_EnglishTitle() + "\"" + ","
                                        "\"" + localVariable.get_ChineseTitle() + "\"" + "," + str(localVariable.get_PrintAtNOK()).capitalize() + "," +
                                        str(localVariable.get_PrintAtOK()).capitalize()  + "," + generate_localVariab_string + "," + "\"" + str(otx_root.get_StatBlock().get_ID()) + "\"" + ")" + "\n")
        # elif localVariable.get_VariableType() == "VciResult":
        #     generate_localVariab_string = (localVariable.get_VariableName() + " = " + localVariable.get_VariableType() + "(" + 
        #                                     value_string + ")" + "\n")
        else:
            generate_localVariab_string = ""
            generate_localVariab_string = (localVariable.get_VariableName() + " = " + value_string + "\n" + generate_localVariab_string)
        LocalVariableList.append(localVariable)
        writeToFile(generate_localVariab_string)
            

def getStepMainFun(otx_root):
    #write the odx server shortname which the script needs
    ODXServerList = []
    LocalVariables = otx_root.LocalVariables.LocalVariable
    for arg in LocalVariables:
        if arg.VariableName.find("ShortName") != -1:
            ODXServerList.append(arg.Value)
    # shortname = "ODXServerList = " + str(ODXServerList) + "\n"
    # writeToFile(shortname)
    #PYPath = os.path.dirname(os.path.abspath(__file__))
    # writeToFile("PYPath = os.path.dirname(os.path.abspath(__file__))\n")
    ##func define
    stepsDefGenerated(otx_root.get_Steps())
    #main func
    # writeToFile('@with_goto\n')
    writeToFile('def main():\n')
    # writeToFile('\tPDUOpenChannel()\n') �ú����������޸�Ϊ�ֶ�����
    writeToFile('\tsignal.signal(signal.SIGUSR1,SignalHandler)\n')
    writeToFile('\tRecordStatblock_(statblockobj)\n')
    stepsNode =otx_root.get_Steps()
    spaceNum = 1
    stepsLabelGenerated(stepsNode, spaceNum)
    # writeToFile('\tPDUCloseChannel()\n')�ú����������޸�Ϊ�ֶ�����
    writeToFile('\tEndCollectInfos_()\n')

    
    
def stepsDefGenerated(stepsNode):
    for stepNode in stepsNode.get_Step():
        baseVariablesNode = stepNode.get_BaseVariables()
        if(baseVariablesNode is not None):
            baseVariablesDefGenerated(stepNode, baseVariablesNode)
        stepsNode = stepNode.get_Steps()
        if(stepsNode is not None):
            stepsDefGenerated(stepsNode)

def stepsLabelGenerated(stepsNode, spaceNum):
#    spaceNum = conditionsGenerated(stepsNode, spaceNum)                                #Conditions Of Steps
    for stepNode in stepsNode.get_Step():
        if stepNode.get_IsActive() == True:
            childSpaceNum = conditionsGenerated(stepNode, spaceNum)
            isCallProcedureStep = False
            if(stepNode.get_FunctionName() == 'CallProcedureStep'):
                isCallProcedureStep = True
            baseVariablesNode = stepNode.get_BaseVariables()
            if(baseVariablesNode is not None):
                if(isCallProcedureStep == True):
                    callProcedureStepGenerated(baseVariablesNode, childSpaceNum)
                else:
                    baseVariablesLabelGenerated(stepNode, baseVariablesNode, childSpaceNum)
            # jumpsNode = stepNode.get_Jumps()
            # if(jumpsNode is not None and isCallProcedureStep is False):#xsdV5 delete Jumps in Step
            #     jumpsGenerated(jumpsNode, childSpaceNum)
            assementNode = stepNode.get_Assement()
            if(assementNode is not None and (assementNode.IsActive) is True):
                assementGenerated(assementNode,childSpaceNum,stepNode, baseVariablesNode)                       
            stepsNode = stepNode.get_Steps()
            if(stepsNode is not None):
                stepsLabelGenerated(stepsNode, childSpaceNum)
            if((baseVariablesNode is not None) and (stepsNode is not None)):
                tkMessageBox.showinfo("warning", stepNode.get_FunctionName() + " both has BaseVariables and Steps!")

def baseVariablesDefGenerated(stepNode, baseVariablesNode):
    generate_para_input = "("
    generate_para_output = "["
    generate_para_input_log = ""
    generate_write_log = "GeneratedParameterType"
    input_parameters = []
    output_parameters = []
    input_parameters_log = []              
    for baseVariableNode in baseVariablesNode.get_BaseVariable():
        baseVariabletosting = ""
        if(baseVariableNode.get_InOutType() == 0 or baseVariableNode.get_InOutType() == 2):
            if baseVariableNode.get_VariableType() == "ecuDtObj":
                baseVariabletosting = (generate_write_log + "(" + baseVariableNode.get_VariableName() + ")")
            elif baseVariableNode.get_VariableType() == "StatStep":
                baseVariabletosting = (generate_write_log + "(" + baseVariableNode.get_VariableName() + ")")
            else:
                baseVariabletosting = (generate_write_log + "(" + baseVariableNode.get_VariableName() + ")")
            input_parameters_log.append(baseVariabletosting)
            input_parameters.append(baseVariableNode.get_VariableName())
        if(baseVariableNode.get_InOutType() == 1 or baseVariableNode.get_InOutType() == 2):
            output_parameters.append(baseVariableNode.get_VariableName())
    output_parameters.append("StpResult")
    for input_str in input_parameters:
        generate_para_input += input_str
        if ((input_parameters.index(input_str) + 1) < len(input_parameters)):
            generate_para_input += ","
    generate_para_input += ")"
    for input_str_log in input_parameters_log:
        generate_para_input_log += input_str_log
        if ((input_parameters_log.index(input_str_log) + 1) < len(input_parameters_log)):
            generate_para_input_log += ","
    for output_str in output_parameters:
        generate_para_output += output_str
        if ((output_parameters.index(output_str) + 1) < len(output_parameters)):
            generate_para_output += ","
    generate_para_output += "]" 
    if len(input_parameters) != 0:
        input_parameters_process = " ".join([ "\", " +  x[(len(generate_write_log) + 1):-1] + " = " + "\"" + " + " + x + " +" for x in generate_para_input_log.split(",")])
        input_parameters_process = input_parameters_process[0]+input_parameters_process[2:-2]
        stepname_guid = stepNode.get_FunctionName() + stepNode.get_GUID()[3:]
        generate_step_code = ("def " + stepname_guid + generate_para_input + ":" + "\n"
                + "\t" + "MSGLogger.debug(" +  "\"" +"Excute " + stepname_guid + "\""  + " + " +  input_parameters_process + ")" + "\n"
                + "\t" + generate_para_output + "=" + stepNode.get_FunctionName() + generate_para_input + "\n"
                + "\t" + "return " + generate_para_output)
    else :
        # input_parameters_process = " ".join([ "\", " +  x[(len(generate_write_log) + 1):-1] + " = " + "\"" + " + " + x + " +" for x in generate_para_input_log.split(",")])
        # input_parameters_process = input_parameters_process[0]+input_parameters_process[2:-2]
        stepname_guid = stepNode.get_FunctionName() + stepNode.get_GUID()[3:]
        generate_step_code = ("def " + stepname_guid + generate_para_input + ":" + "\n"
                + "\t" + "MSGLogger.debug(" +  "\"" +"Excute " + stepname_guid + "\""   + ")" + "\n"
                + "\t" + generate_para_output + "=" + stepNode.get_FunctionName() + generate_para_input + "\n"
                + "\t" + "return " + generate_para_output)
    writeToFile(generate_step_code + "\n\n")

def executeatGenerated(conditionNode):
    excuteattype =  conditionNode.get_ExcuteAt()
    if excuteattype == "Always":
        return ""
    if excuteattype == "If":
        return "if("
    if excuteattype == "While":
        return "while("

def conditionsGenerated(parentNode, spaceNum):
    conditionsNode = parentNode.get_Conditions()
    if(conditionsNode is not None):
        stepname_guid = ''
#        stepname_guid = '\t' * spaceNum + 'label.' + parentNode.get_GUID() + '\n'     #xsdv5 cancle goto
        expressionText = ''
        LASExpression = ''
        previousResultText = ''
        for conditionNode in conditionsNode.get_Condition():
            if conditionNode.get_IsActive() != True:
                continue
            conditionType = conditionNode.get_Type()
            if(conditionType == 'LAS'):#LASExpression
                lasRefNode = conditionNode.get_LASRef()
                if(lasRefNode.get_LasId() is None):
                    LASExpression = 'True'
                    continue
                for lasVariable in lasVariablesList:
                    if(lasVariable.get_GUID() == lasRefNode.get_LasId() and lasVariable.get_LASExpression() is not None):
                        LASExpression = (lasVariable.get_LASExpression())[1:]
                ##find lasExpression in LocalVariables after localVariables done
                LASExpression = '\t' * spaceNum + 'if(' + 'Evaluate(' +  "'" + LASExpression + "'" + ')' + '):' + '\n'
            elif(conditionType == 'Programtic'):
                expressionNode = conditionNode.get_Expression()
                expressionText = expressionNode.get_Value()
                if expressionText == '' or expressionText == 'true':
                    expressionText = 'True'
                programticres = executeatGenerated(conditionNode)
                if(programticres == ""):
                    expressionText = ""
                else:
                    expressionText = '\t' * spaceNum + programticres + expressionText + '):' + '\n'
#                expressionText = '\t' * spaceNum + 'if(' + expressionText + '):' + '\n'
            elif(conditionType == 'PreviousResult'):
                previousResultValue = conditionNode.get_PreviousResult() #if head of property was StepResult
                if(previousResultValue == 'OK'):
                    previousResultText = 'StpResult==StepResult.Ok'
                elif(previousResultValue == 'NOK'):
                    previousResultText = 'StpResult==StepResult.Nok'
                elif(previousResultValue == 'ABORT'):
                    previousResultText = 'StpResult==StepResult.Abort'
                else: #previousResultValue == 'None' || ''
                    previousResultText = 'True'
                previousresultres = executeatGenerated(conditionNode)
                if(previousresultres == ""):
                    previousResultText = ""
                else:
                    previousResultText = '\t' * spaceNum + previousresultres + previousResultText + '):' + '\n'                
#                previousResultText = '\t' * spaceNum + 'if(' + previousResultText + '):' + '\n'
            spaceNum = spaceNum + 1
        generate_step_code = (stepname_guid + LASExpression + previousResultText + expressionText)
        writeToFile(generate_step_code)
    return spaceNum

def baseVariablesLabelGenerated(stepNode, baseVariablesNode, spaceNum):
    generate_para_input = '('
    generate_para_output = '['
    input_parameters = []
    output_parameters = []
    input_index = 1
    output_index = 1
    for baseVariableNode in baseVariablesNode.get_BaseVariable():
        if(baseVariableNode.get_InOutType() == 0 or baseVariableNode.get_InOutType() == 2):
            input_parameters = checkInputValueType(input_parameters, baseVariableNode)
            if(stepNode.get_FunctionName() == 'SetValue'):
                setValueDict.update({baseVariableNode.get_VariableName():baseVariableNode.get_Value()})
        if(baseVariableNode.get_InOutType() == 1 or baseVariableNode.get_InOutType() == 2):
            output_parameters = checkOutputValueType(output_parameters, baseVariableNode)
    output_parameters.append('StpResult')

    for input_str in input_parameters:
        if isinstance(input_str,dict):
            generate_para_input += '{'
            for key in input_str:
                generate_para_input += "'"+key+"'"+':'+"'"+input_str[key]+"'"
                generate_para_input += ','
            generate_para_input = generate_para_input.removesuffix(',')
            generate_para_input += '}'
        elif isinstance(input_str,str):
            generate_para_input += input_str
        if (input_index != len(input_parameters)):
            generate_para_input += ','
            input_index = input_index + 1
    generate_para_input += ')'
    for output_str in output_parameters:
        generate_para_output += output_str
        if (output_index != len(output_parameters)):
            generate_para_output += ','
            output_index = output_index + 1
    generate_para_output += ']'
    stepname_guid = stepNode.get_FunctionName() + stepNode.get_GUID()[3:]
    generate_step_code = ('\t' * spaceNum + generate_para_output + '= ' + stepname_guid + generate_para_input + '\n')
    writeToFile(generate_step_code)
    
def checkInputValueType(parameters, baseVariableNode):
    if(baseVariableNode.get_IsByBinding() == 'true'):
        for localVariable in LocalVariableList:
            if localVariable.get_GUID() == baseVariableNode.get_BindLocalVarGUID():
                parameters.append(localVariable.get_VariableName())
                return parameters
    if(baseVariableNode.get_VariableType() == 'bool'):
        if(baseVariableNode.get_Value() != 'true'):
            parameters.append('False')
        else:
            parameters.append('True')
    elif(baseVariableNode.get_VariableType() == 'int'):
        if(baseVariableNode.get_Value().isdigit()):
            parameters.append(baseVariableNode.get_Value())
        else:
            parameters.append('0')
    elif(baseVariableNode.get_VariableType() == 'string'):
        #parameters.append("'" + baseVariableNode.get_Value() + "'") because testCCU.xml emerge that the fun'inputparam is a string not a variable ,so change the line of codes
        parameters.append(baseVariableNode.get_Value())
    elif(baseVariableNode.get_VariableType() == 'ecuDtObj' or baseVariableNode.get_VariableType() == 'vciResult' or baseVariableNode.get_VariableType() == 'StatStep'):
        if(baseVariableNode.get_Value() == ''):
            parameters.append('None')
        else:
            parameters.append(baseVariableNode.get_Value())
    elif(baseVariableNode.get_VariableType() == 'Dict'):    #chapter 11
        parameters.append(setValueDict)
    else:
        parameters.append(baseVariableNode.get_Value())
    return parameters

def checkOutputValueType(parameters, baseVariableNode):
    if(baseVariableNode.get_IsByBinding() == 'true'):
        for localVariable in LocalVariableList:
            if localVariable.get_GUID() == baseVariableNode.get_BindLocalVarGUID():
                parameters.append(localVariable.get_VariableName())
                return parameters
    if baseVariableNode.get_VariableType() == 'Dict':       #chapter 11
        parameters.append('outParamDict')
    else:
        if baseVariableNode.get_Value() == '':
            if baseVariableNode.get_IsOptional() == True:
                parameters.append('UnneededOutParam')
            else:
                parameters.append('EmptyOutParam')
        else:
            parameters.append(baseVariableNode.get_Value())
    return parameters
    
def callProcedureStepGenerated(baseVariablesNode, spaceNum):
    uuid = ''
    for baseVariableNode in baseVariablesNode.get_BaseVariable():
        if(baseVariableNode.get_InOutType() == 0 or baseVariableNode.get_InOutType() == 2):
            uuid = baseVariableNode.get_Value()
            callProcedureStepList.append(uuid)
            procedureNode = callProcedureStepFunc(uuid)
            title = procedureNode.get_Title()
    generate_step_code = ('\t' * spaceNum + 'os.popen(' + "'" + title + '_' + uuid + '.py' + "'" + ')' + '\n')
    writeToFile(generate_step_code)
    
def jumpsGenerated(jumpsNode, spaceNum):
    generate_step_code = ''
    ifOkJumpNode = jumpsNode.get_IfOkJump()
    generate_step_code = generate_step_code + ('\t' * (spaceNum) + 'if(StpResult==StepResult.Ok):\n') + '\t' * (spaceNum+1) + ('goto.') + ifOkJumpNode.get_TargetStepGUID() + '\n'
    ifNokJumpNode = jumpsNode.get_IfNokJump()    
    generate_step_code = generate_step_code + ('\t' * (spaceNum) + 'if(StpResult==StepResult.Nok):\n') + '\t' * (spaceNum+1) + ('goto.') + ifNokJumpNode.get_TargetStepGUID() + '\n'
    writeToFile(generate_step_code)

def assementGenerated(assementNode,spaceNum,stepNode, baseVariablesNode):
    generate_assement_code = ""    
    if(assementNode.Action == "Cancel"):
        if(assementNode.IsAssmentAtOKEnable == True and assementNode.IsAssmentAtNOKEnable == True):
            generate_assement_code = '\t' * (spaceNum) + "if(StpResult==StepResult.Ok or StpResult==StepResult.Nok):\n" + '\t' * (spaceNum+1) + "return StpResult\n"
            writeToFile(generate_assement_code)
    if(assementNode.Action == "Repeat"):
        if(assementNode.IsAssmentAtOKEnable == True and assementNode.IsAssmentAtProgramaticEnable == True):
            generate_assement_code = '\t' * (spaceNum) + "while(StpResult==StepResult.Ok or " + assementNode.ProgramaticAssement + "):" + "\n"
            writeToFile(generate_assement_code)
            baseVariablesLabelGenerated(stepNode, baseVariablesNode, spaceNum + 1)
    if((assementNode.Action) == "Terminate"):
        if(assementNode.IsAssmentAtOKEnable == True and assementNode.IsAssmentAtAbortEnable == True):
            generate_assement_code = '\t' * (spaceNum) + "if(StpResult==StepResult.Ok or StpResult==StepResult.Abort):\n" + '\t' * (spaceNum+1) + "exit()\n"
            writeToFile(generate_assement_code)
            
    
def generateProcedure(otx_root):
    global pythonFileName
    pythonFileName = otx_root.get_Title() + '_' + otx_root.get_GUID() #uuid of test.xml contains name and uuid

    #Generate Annotation
    getAnnotationFun(otx_root)
    
    #Generate ImportLib
    getImportLib(otx_root)

    generatedStatBlock(otx_root)

    #Generate LocalVariables
    getLocalVariableFun(otx_root)
    
    #Generate step interface   
    getStepMainFun(otx_root)
    
    #Main func
    generateMainFunc()
    
    for callProcedureStep in callProcedureStepList: #generate other xmls
        procedureNode = callProcedureStepFunc(callProcedureStep)
        callProcedureStepList.remove(callProcedureStep)
        generateProcedure(procedureNode)
        
def generateMainFunc():
    writeToFile("if ( __name__ == " + "\"" + "__main__" + "\"" + "):\n"
    + "\tmain()")
    
def callProcedureStepFunc(guid):
    global parentDirector
    files = os.listdir(parentDirector)
    for file in files:
        if(file.endswith('.xml')):
            procedureNode = xsdToPython.parse(parentDirector+ '\\' + file)
            if(procedureNode.get_GUID() == guid):
                return procedureNode
    
def writeToFile(generate_string):
    generate_file = open('GeneratedCode/' + pythonFileName + '.py','a+',encoding="utf-8")
    generate_file.write(generate_string)
    
