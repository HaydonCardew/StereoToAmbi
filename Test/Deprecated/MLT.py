import ctypes
import numpy.ctypeslib as ctl
import numpy as np
import soundfile as sf
import matplotlib.pyplot as plt

_c_nddouble = ctl.ndpointer(np.float64, flags='aligned, c_contiguous')
_c_ndfloat = ctl.ndpointer(np.float32, flags='aligned, c_contiguous')
_c_ndint = ctl.ndpointer(ctypes.c_int, flags='aligned, c_contiguous')

class MultiLevelThreshold(object):
    def __init__(self):
        self.lib = ctypes.cdll.LoadLibrary('/Users/haydoncardew/Desktop/GoogleDrive-Local/Plugin-Testing/TestMultiLevelThreshold/Builds/MacOSX/build/Debug/TestMultiLevelThreshold.dylib')
        self._decorateFunctions()
        self.obj = self.lib.MT_new(3, 1024, 100, 3)

    def __del__(self):
        self.lib.MT_delete(self.obj)

    def mlt(self, data, nDataPoints, thresholds, nThresholds, nHistBins):
        data = np.ascontiguousarray(data, dtype=np.float32)
        thresholds = np.ascontiguousarray(thresholds, dtype=np.float32)
        self.lib.MT_multiLevelThreshold(self.obj, data, nDataPoints, thresholds, nThresholds, nHistBins)
        return thresholds

    def _setTypes(self, fn, returntype, argtypes):
        fn.restype = returntype
        fn.argtypes = argtypes

    def _decorateFunctions(self):
        self._setTypes(self.lib.MT_new, ctypes.c_void_p, [ctypes.c_int32, ctypes.c_int32, ctypes.c_int32, ctypes.c_int32])
        self._setTypes(self.lib.MT_multiLevelThreshold, None, [ctypes.c_void_p, _c_ndfloat, ctypes.c_int32, _c_ndfloat, ctypes.c_int32, ctypes.c_int32])
        self._setTypes(self.lib.MT_delete, None, [ctypes.c_void_p])

x = MultiLevelThreshold()
nThresholds = 3
thresholds = np.zeros((nThresholds))
nDataPoints = 153
data = np.arange(nDataPoints)
nBins = 100
thresholds = x.mlt(data, nDataPoints, thresholds, nThresholds, nBins)
print(data)
print(thresholds)

print('finito')