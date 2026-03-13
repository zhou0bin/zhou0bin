import io

fileInfo = io.open('xsdToPython.py', 'r+',encoding='UTF-8')
data = fileInfo.read()
## patch to replace str
data = data.replace("self.validate_IDType_patterns_, value", "self.validate_IDType_patterns_, str(value)")
##
fileInfo.seek(0)
fileInfo.write(data)
fileInfo.close()
    