from .utils.db_data import DbVehicleData
from .utils.util_helper import utilVehicleDataFile
from .log_info import MSGLogger,ToolsFunctionDecorator
@ToolsFunctionDecorator(rcount=1)
def GetMtoc():
    try:
        DbVehicle = DbVehicleData(utilVehicleDataFile())
        return DbVehicle.getMTOC()
    except Exception as exception:
        MSGLogger.error({'错误':'获取MTOC码失败'})
        return ''