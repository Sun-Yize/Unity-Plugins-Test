import ctypes


libaddition = ctypes.CDLL('./dylib_cpp/libaddition.dylib')
libaddition.add.argtypes = [ctypes.c_double, ctypes.c_double]
libaddition.add.restype = ctypes.c_double

result = libaddition.add(3.5, 4.5)
print(f"Result: {result}")
