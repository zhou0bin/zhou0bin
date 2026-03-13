
from enum import Enum
StepResult = Enum('StepResult', ('Ok', 'Nok', 'Cancel', 'None','Abort'))


def BlankStep():
    return StepResult.Ok