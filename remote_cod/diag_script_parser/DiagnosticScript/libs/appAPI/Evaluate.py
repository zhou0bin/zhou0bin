
from .log_info import MSGLogger
from .utils.db_data import DbVehicleData
from .utils.util_helper import utilVehicleDataFile
import re
#判断 LAS 表达式真假
def Evaluate(LasExpression:str,LasList=None):
    try:
        if type(LasExpression) != str or LasExpression == None:
            MSGLogger.error("Evaluate inputparam is error")
            return False
        LasExpressionValue = LasExpression
        TempLasStr = ''
        Result = False
        #删除LasExpression中的空格,左右括号,英文加号，英文感叹号，英文句号，赋值给TempLasStr，
        for ch in LasExpressionValue:
            if ch == ' ' or ch == '(' or ch == ')' or ch == '+' or ch == '!' or ch == '.' or ch == '$':
                pass
            else:
                TempLasStr += ch
        #从车辆数据中获取JSON数据
        if LasList==None or LasList=="":
            DbVehicle = DbVehicleData(utilVehicleDataFile())
            LasList = DbVehicle.getLAS()
        #TempLasStr的长度能被4整除？
        if not TempLasStr or (not LasList) or (len(TempLasStr)%4 != 0):
            return False
        else:
            #将TempLasStr按照4个一组，从开始位置依次截取，构成一个4字符串数组TempLasList，数组长度为N
            cut = lambda obj,sec: [obj[i:i+sec] for i in range(0,len(obj),sec)]
            TempLasList = cut(TempLasStr,4)
            #依次取出TempLasList中的每个LAS，判断是否属于集合LasList
            for ch in TempLasList:
                if ch in LasList:
                    LasExpressionValue = re.sub(ch, 'True', LasExpressionValue)
                else:
                    LasExpressionValue = re.sub(ch, 'False', LasExpressionValue)
            #将LASExpression视为逻辑表达式，计算LasExpression 将结果True或者False赋值给Result并返回
            LasExpressionValue = LasExpressionValue.replace('+',' or ').replace('.',' and ').replace( '!',' not ').replace('$','')
            Result = eval(LasExpressionValue)
    except:
        Result = False
    return Result
# def Evaluate(LasExpression:str):
#     try:
#         if type(LasExpression) != str or LasExpression == None:
#             MSGLogger.error("Evaluate inputparam is error")
#             return False
#         LasExpressionValue = LasExpression
#         TempLasStr = ''
#         Result = False
#         #删除LasExpression中的空格,左右括号,英文加号，英文感叹号，英文句号，赋值给TempLasStr，
#         for ch in LasExpressionValue:
#             if ch == ' ' or ch == '(' or ch == ')' or ch == '+' or ch == '!' or ch == '.' or ch == '$':
#                 pass
#             else:
#                 TempLasStr += ch
#         #从车辆数据中获取JSON数据
#         DbVehicle = DbVehicleData(utilVehicleDataFile())
#         LasList = DbVehicle.getLAS()
#         #TempLasStr的长度能被4整除？
#         if not TempLasStr or (not LasList) or (len(TempLasStr)%4 != 0):
#             return False
#         else:
#             #将TempLasStr按照4个一组，从开始位置依次截取，构成一个4字符串数组TempLasList，数组长度为N
#             cut = lambda obj,sec: [obj[i:i+sec] for i in range(0,len(obj),sec)]
#             TempLasList = cut(TempLasStr,4)
#             #依次取出TempLasList中的每个LAS，判断是否属于集合LasList
#             for ch in TempLasList:
#                 if ch in LasList:
#                     LasExpressionValue = re.sub(ch, 'True', LasExpressionValue)
#                 else:
#                     LasExpressionValue = re.sub(ch, 'False', LasExpressionValue)
#             #将LASExpression视为逻辑表达式，计算LasExpression 将结果True或者False赋值给Result并返回
#             LasExpressionValue = LasExpressionValue.replace('+',' or ').replace('.',' and ').replace( '!',' not ').replace('$','')
#             Result = eval(LasExpressionValue)
#     except:
#         Result = False
#     return Result

# @handler_exception_decorator(length=1)
# def Evaluate(LasExpression:str):
#     return Evaluate_p(LasExpression)