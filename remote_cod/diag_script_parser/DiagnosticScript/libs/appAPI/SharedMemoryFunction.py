from .log_info import ToolsFunctionDecorator
_global_variable = {}

@ToolsFunctionDecorator(rcount=0)
def SharedMemoryPutString(Token:str,string:str):
    global _global_variable
    if Token != None:
        _global_variable[Token] = string

@ToolsFunctionDecorator(rcount=1)
def SharedMemoryGetString(Token:str):
    global _global_variable
    if Token in _global_variable.keys():
        return _global_variable[Token]
    else:
        return ""

@ToolsFunctionDecorator(rcount=1)
def SharedMemoryContainsKey(Token:str):
    global _global_variable
    if Token in _global_variable.keys():
        return True
    else:
        return False

@ToolsFunctionDecorator(rcount=0)
def SharedMemoryPutObject(Token:str,obj):
    global _global_variable
    if Token != None:
        _global_variable[Token] = obj

@ToolsFunctionDecorator(rcount=1)    
def SharedMemoryGetObject(Token:str):
    global _global_variable
    if Token in _global_variable.keys():
        return _global_variable[Token]
    else:
        return None
    

