from .utils.db_data import DbVehicleData
from .utils.util_helper import utilVehicleDataFile
from .log_info import MSGLogger,handler_exception_decorator
@handler_exception_decorator(length=1)
def GetVin():
    try:
        DbVehicle = DbVehicleData(utilVehicleDataFile())
        return DbVehicle.getVIN()
    except Exception as exception:
        MSGLogger.error({'错误':'获取VIN码失败'})
        return ''