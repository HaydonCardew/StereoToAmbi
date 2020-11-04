import ctypes
import numpy.ctypeslib as ctl
import numpy as np
import soundfile as sf
import matplotlib.pyplot as plt
import os

_c_nddouble = ctl.ndpointer(np.float64, flags='aligned, c_contiguous')
_c_ndfloat = ctl.ndpointer(np.float32, flags='aligned, c_contiguous')
_c_ndint = ctl.ndpointer(ctypes.c_int, flags='aligned, c_contiguous')

class StereoToAmbiAudioProcessor(object):
    def __init__(self, nThresholds):
        self.lib = ctypes.cdll.LoadLibrary('TestMultiLevelThreshold/Builds/MacOSX/build/Debug/TestMultiLevelThreshold.dylib')
        self._decorateFunctions()
        self.obj = self.lib.STAAP_new(nThresholds)

    def __del__(self):
        self.lib.STAAP_delete(self.obj)

    def write(self, left, right, nSamples):
        self.lib.STAAP_write(self.obj, np.ascontiguousarray(left, dtype=np.float32), np.ascontiguousarray(right, dtype=np.float32), nSamples)

    def multiRead(self, buffer, nSamples, azimuths, width, sr):
        buffer = np.ascontiguousarray(buffer, dtype=np.float32)
        azimuths = np.ascontiguousarray(azimuths, dtype=np.float32)
        nRead = self.lib.STAAP_multi_read(self.obj, buffer, nSamples, azimuths, width, sr)
        return buffer, nRead, azimuths

    def deverbWrite(self, left, right, nSamples):
        self.lib.STAAP_deverb_write(self.obj, np.ascontiguousarray(left, dtype=np.float32), np.ascontiguousarray(right, dtype=np.float32), nSamples)

    def deverbRead(self, buffer, nSamples):
        buffer = np.ascontiguousarray(buffer, dtype=np.float32)
        nRead = self.lib.STAAP_deverb_read(self.obj, buffer, nSamples)
        return buffer, nRead

    def getHisto(self, bins, probs, size):
        bins = np.ascontiguousarray(bins, dtype=ctypes.c_int)
        probs = np.ascontiguousarray(probs, dtype=np.float32)
        self.lib.STAAP_getLastHisto(self.obj, probs, bins, size)
        return bins, probs

    def _setTypes(self, fn, returntype, argtypes):
        fn.restype = returntype
        fn.argtypes = argtypes

    def _decorateFunctions(self):
        self._setTypes(self.lib.STAAP_new, ctypes.c_void_p, [ctypes.c_int32])
        self._setTypes(self.lib.STAAP_write, None, [ctypes.c_void_p, _c_ndfloat, _c_ndfloat, ctypes.c_int32])
        self._setTypes(self.lib.STAAP_multi_read, ctypes.c_int32, [ctypes.c_void_p, _c_ndfloat, ctypes.c_int32, _c_ndfloat, ctypes.c_float, ctypes.c_int32])
        #self._setTypes(self.lib.STAAP_getLastHisto, None, [ctypes.c_void_p, _c_ndfloat, _c_ndint, ctypes.c_int32])
        self._setTypes(self.lib.STAAP_deverb_write, None, [ctypes.c_void_p, _c_ndfloat, _c_ndfloat, ctypes.c_int32])
        self._setTypes(self.lib.STAAP_deverb_read, ctypes.c_int32, [ctypes.c_void_p, _c_ndfloat, ctypes.c_int32])
        self._setTypes(self.lib.STAAP_delete, None, [ctypes.c_void_p])

blockSize = 512
nTotalWritten = 0
nTotalRead = 0

x = StereoToAmbiAudioProcessor(1)

dualMono = True

folder = "DeverbTest"
filename = f"{folder}/Both.wav"

# get audio size
f = sf.SoundFile(filename)
samplerate = f.samplerate
nTotalSamples = len(f)

nSources = 4
outTmp = np.zeros((blockSize * nSources))

outData = np.zeros((nTotalSamples, nSources))
directAudio = np.zeros((nTotalSamples, 2))
ambientAudio = np.zeros((nTotalSamples, 2))

percentageJump = 10
progress = percentageJump

for block in sf.blocks(filename, blocksize=blockSize, overlap=0, dtype='float32'):
    if dualMono:
        x.deverbWrite(block[:,0], block[:,0], blockSize)
    else:
        x.deverbWrite(block[:,0], block[:,1], blockSize)
    
    nTotalWritten = nTotalWritten + blockSize
    outTmp, nRead = x.deverbRead(outTmp, blockSize)

    if(nRead != 0):
        i = 0
        directAudio[nTotalRead:(nTotalRead+nRead), 0] = outTmp[(i*blockSize):((i+1)*blockSize)]
        i = 1
        directAudio[nTotalRead:(nTotalRead+nRead), 1] = outTmp[(i*blockSize):((i+1)*blockSize)]
        i = 2
        ambientAudio[nTotalRead:(nTotalRead+nRead), 0] = outTmp[(i*blockSize):((i+1)*blockSize)]
        i = 3
        ambientAudio[nTotalRead:(nTotalRead+nRead), 1] = outTmp[(i*blockSize):((i+1)*blockSize)]

    if((nTotalRead+nRead) >= nTotalSamples):
        diff = (nTotalRead+nRead) - nTotalSamples
        break
    nTotalRead = nRead + nTotalRead
    if(((float(nTotalRead)/float(nTotalSamples)) * 100) > progress):
        print('Completed: {}%'.format(progress))
        progress = progress + percentageJump

name = f'{folder}/Ambient.wav'
sf.write(name, ambientAudio, samplerate)

name = f'{folder}/Direct.wav'
sf.write(name, directAudio, samplerate)

print("Finito")