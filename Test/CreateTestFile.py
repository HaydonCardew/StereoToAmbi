import soundfile as sf
import numpy as np

outFile = "TestAudio/PannedSources.wav"
length = 10
samplerate = 96000

filename1 = "TestAudio/Treble.wav"
pos1 = -100

filename2= "TestAudio/Tenor.wav"
pos2 = 170

filename3 = "TestAudio/Bass.wav"
pos3 = 10

def getPannedSource(filename, length, pan):
    data, samplerate = sf.read(filename)
    x = np.zeros((length*samplerate,2))
    pan = (float(pan)+180)/360
    pan = pan*(np.pi/2)
    x[:,0] = np.cos(pan)*data[0:length*samplerate]
    x[:,1] = np.sin(pan)*data[0:length*samplerate]
    return x

outData = getPannedSource(filename1, length, pos1) + getPannedSource(filename2, length, pos2) + getPannedSource(filename3, length, pos3) 

sf.write(outFile, outData, samplerate)