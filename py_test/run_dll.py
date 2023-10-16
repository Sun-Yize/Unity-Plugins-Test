import ctypes


libaddition= ctypes.CDLL("./dll_cpp/x64/Release/dll_cpp.dll")
# libaddition = ctypes.CDLL("./dll_cpp/ARM/Release/dll_cpp.dll")

result = libaddition.add(1, 1)
print(f"Result: {result}")
