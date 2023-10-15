from ctypes import *


pDll = CDLL("./dll_cpp/x64/Release/dll_cpp.dll")
# pDll = CDLL("./dll_cpp/ARM/Release/dll_cpp.dll")

res = pDll.add(1, 1)
print(res)
