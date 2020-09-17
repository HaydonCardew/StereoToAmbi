import ctypes
import numpy.ctypeslib as ctl
import numpy as np
import soundfile as sf
import matplotlib.pyplot as plt
import os

_c_nddouble = ctl.ndpointer(np.float64, flags='aligned, c_contiguous')
_c_ndfloat = ctl.ndpointer(np.float32, flags='aligned, c_contiguous')
_c_ndint = ctl.ndpointer(ctypes.c_int, flags='aligned, c_contiguous')

class WindowedFIFO(object):
    def __init__(self, windowSize, overlap):
        self.lib = ctypes.cdll.LoadLibrary('TestMultiLevelThreshold/Builds/MacOSX/build/Debug/TestMultiLevelThreshold.dylib')
        self._decorateFunctions()
        self.obj = self.lib.WFIFOB_new(windowSize, overlap)

    def __del__(self):
        self.lib.WFIFOB_delete(self.obj)

    def write(self, data, nSamples, gain=1.0):
        self.lib.WFIFOB_write(self.obj, np.ascontiguousarray(data, dtype=np.float32), nSamples, gain)

    def read(self, data, nSamples):
        data = np.ascontiguousarray(data, dtype=np.float32)
        nRead = self.lib.WFIFOB_read(self.obj, data, nSamples, False)
        return data, nRead

    def sendWindow(self, windowedData):
        self.lib.WFIFOB_sendProcessedWindow(self.obj, np.ascontiguousarray(windowedData, dtype=np.float32))

    def getWindow(self, windowedData):
        windowedData = np.ascontiguousarray(windowedData, dtype=np.float32)
        nRead = self.lib.WFIFOB_getWindowedAudio(self.obj, windowedData)
        return windowedData, nRead

    def _setTypes(self, fn, returntype, argtypes):
        fn.restype = returntype
        fn.argtypes = argtypes

    def _decorateFunctions(self):
        self._setTypes(self.lib.WFIFOB_new, ctypes.c_void_p, [ctypes.c_int32, ctypes.c_float])
        self._setTypes(self.lib.WFIFOB_write, None, [ctypes.c_void_p, _c_ndfloat, ctypes.c_int32, ctypes.c_float])
        self._setTypes(self.lib.WFIFOB_read, ctypes.c_int32, [ctypes.c_void_p, _c_ndfloat, ctypes.c_int32, ctypes.c_bool])
        self._setTypes(self.lib.WFIFOB_sendProcessedWindow, None, [ctypes.c_void_p, _c_ndfloat])
        self._setTypes(self.lib.WFIFOB_getWindowedAudio, ctypes.c_int32, [ctypes.c_void_p, _c_ndfloat])
        self._setTypes(self.lib.WFIFOB_delete, None, [ctypes.c_void_p])

windowSize = 4096 # what is used in stoa
overlap = 0.5
x = WindowedFIFO(windowSize, overlap)

# set up to print graph of 2nd read for any overlap and see how close to one it is
inputBufferSize = windowSize * 3
inputBuffer = np.ones( windowSize )
x.write(inputBuffer, inputBufferSize, gain=0.9)

window = np.zeros(windowSize)
window, nRead = x.getWindow(window)

for i in range(10):
    x.sendWindow(window)

pre = np.zeros(windowSize)
pre, nRead = x.read(pre, windowSize)

output = np.zeros(windowSize)
output, nRead = x.read(output, windowSize)

plt.plot(output)
plt.show()
print("finito")