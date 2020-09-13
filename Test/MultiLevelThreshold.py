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
        self._setTypes(self.lib.STAAP_multi_read, ctypes.c_int32, [ctypes.c_void_p, _c_ndfloat, ctypes.c_int32, _c_ndfloat, ctypes.c_float])
        #self._setTypes(self.lib.STAAP_getLastHisto, None, [ctypes.c_void_p, _c_ndfloat, _c_ndint, ctypes.c_int32])
        self._setTypes(self.lib.STAAP_delete, None, [ctypes.c_void_p])

'''
stftWindow = 0.180;
wlen = ceil(stftWindow * fs) = 17280 (@96k);
zeroPadSize = ceil(zeroPadSize * wlen) = ;
nfft = wlen + zeroPadSize - 1 = 34k;
nfft = 2 ^ nextpow2(nfft) = ;
'''

width = 360
blockSize = 512
nTotalWritten = 0
nTotalRead = 0
nThresholds = 1

nSources = (nThresholds+1)*2

x = StereoToAmbiAudioProcessor(nThresholds)

filename = 'TestAudio/PannedSources.wav'
#filename = 'TestAudio/Test.wav'
filename = 'TestAudio/2L.flac'
# get audio size
f = sf.SoundFile(filename)
samplerate = f.samplerate
nTotalSamples = len(f)

outTmp = np.zeros((blockSize * nSources))
outData = np.zeros((nTotalSamples, nSources))

azimuthsTmp = np.zeros((1, nSources))
azimuths = np.zeros((int(nTotalSamples/blockSize)+1, nSources))

histoSize = 100
histoBins = np.zeros((1, histoSize))
histoProbs = np.zeros((1, histoSize))

azimuthCount = 0

percentageJump = 10
progress = percentageJump

for block in sf.blocks(filename, blocksize=blockSize, overlap=0, dtype='float32'):
    x.write(block[:,0], block[:,1], blockSize)
    nTotalWritten = nTotalWritten + blockSize
    outTmp, nRead, azimuthsTmp = x.multiRead(outTmp, blockSize, azimuthsTmp, width, samplerate)
    if(nRead != 0):
        #histoBins, histoProbs = x.getHisto(histoBins, histoProbs, histoSize)
        azimuths[azimuthCount,:] = azimuthsTmp
        azimuthCount = azimuthCount+1
        for i in range(nSources):
            j = outTmp[(i*blockSize):((i+1)*blockSize)]
            outData[nTotalRead:(nTotalRead+nRead),i] = j
    if((nTotalRead+nRead) >= nTotalSamples): 
        diff = (nTotalRead+nRead) - nTotalSamples
        #outData[nTotalRead:(nTotalRead+diff),:] = outTmp[0:diff,:]
        #for i in range(nSources):
        #    outData[nTotalRead:(nTotalRead+diff),i] = outTmp[(i*blockSize):(i+1)*blockSize]
        break
    nTotalRead = nRead + nTotalRead
    if(((float(nTotalRead)/float(nTotalSamples)) * 100) > progress):
        print('Completed: {}%'.format(progress))
        progress = progress + percentageJump
    #print('Completed: {}/{}'.format(nTotalRead, nTotalSamples))

nAziPoints = len(azimuths)
t = range(nAziPoints)
plt.figure(1)
plt.plot(t, azimuths)
plt.xlabel('Sample Blocks')
plt.ylabel('Angle')
plt.title('Extracted Source Azimuths')
plt.grid(True)
if not os.path.exists("Graphs"):
    os.makedirs("Graphs")
plt.savefig("Graphs/Azimuths.png")
#plt.show()
for i in range(nSources):
    out = 'Azimuth {} avg = {}'.format(i, np.mean(azimuths[:,i]))
    print(out)

for i in range(nSources):
    name = 'TestAudio/testOut{}.wav'.format(i)
    sf.write(name, outData[:,i], samplerate)
print("Finito")