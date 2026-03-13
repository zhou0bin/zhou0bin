class LocalVariable:
    def __init__(self, BaseDataType,GUID,Value,VariableType,DefaultValue):
        self.BaseDataType = BaseDataType
        self.GUID = GUID
        self.Value = Value
#        self.InOutType = InOutType
        self.VariableType = VariableType
        self.DefaultValue = DefaultValue        

class ecuDtObj(LocalVariable):
    def __init__(self, Name,RequestID,ResponseID,FrameType,BaseDataType,GUID,Value,VariableType,DefaultValue):
        self.Name = Name
        self.RequestID = RequestID
        self.ResponseID = ResponseID
        self.FrameType = FrameType
        super().__init__(BaseDataType,GUID,Value,VariableType,DefaultValue)
        
class LAS(LocalVariable):
    def __init__(self, LASExpression,BaseDataType,GUID,Value,VariableType,DefaultValue):
        self.LASExpression = LASExpression
        super().__init__(BaseDataType,GUID,Value,VariableType,DefaultValue)
        
class StatStep(LocalVariable):
    def __init__(self, ID,EnglishTranslation,ChineseTranslation,PrintAtNok,PrintAtOK,StatBlock,BaseDataType,GUID,Value,VariableType,DefaultValue):
        self.ID = ID
        self.EnglishTranslation = EnglishTranslation
        self.ChineseTranslation = ChineseTranslation
        self.PrintAtNok = PrintAtNok
        self.PrintAtOK = PrintAtOK
        self.StatBlock = StatBlock
        super().__init__(BaseDataType,GUID,Value,VariableType,DefaultValue)
        
# class VciResult(LocalVariable):
#     def __init__(self, BaseDataType,GUID,Value,VariableType,DefaultValue):
#         super().__init__(BaseDataType,GUID,Value,VariableType,DefaultValue)

class StatBlock():
    def __init__(self,ID,EnglishTitle,ChineseTitle,PrintAtNOK,PrintAtOK):
        self.ID = ID
        self.EnglishTitle = EnglishTitle
        self.ChineseTitle = ChineseTitle
        self.PrintAtNOK = PrintAtNOK
        self.PrintAtOK = PrintAtOK
        
def GeneratedParameterType(input_parameter):
     if input_parameter is None:
         return ""    
     elif type(input_parameter) is str:
         return input_parameter
     elif type(input_parameter) is ecuDtObj:
         return str(input_parameter.Name)
     elif type(input_parameter) is StatStep:
         return str(input_parameter.Value)
     else:
         return str(input_parameter)