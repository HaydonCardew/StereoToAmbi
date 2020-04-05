import soundfile as sf
import numpy as np

filename = 'TestAudio/Test.wav'

samplerate = 48000
length = 10 # seconds
maxValue = 0.25

def createSineWave(freq, length, samplerate):
    # length in secs
    x = np.arange(length*samplerate)
    y = maxValue * np.sin(2 * np.pi * freq * x / samplerate)
    return y

def createPannedSineWave(freq, length, samplerate, pan):
    # Equal power pan law
    # Pan maps from -180 -> 180
    x = np.zeros((length*samplerate,2))
    y = createSineWave(freq, length, samplerate)
    pan = (float(pan)+180)/360
    pan = pan*(np.pi/2)
    x[:,0] = np.cos(pan)*y
    x[:,1] = np.sin(pan)*y
    return x

def createMovingPannedSineWave(freq, length, samplerate, startPan, endPan):
    x = np.zeros((length*samplerate,2))
    startPan = (startPan+180)/360
    startPan = startPan*(np.pi/2)
    lStartPan = np.cos(startPan)
    rStartPan = np.sin(startPan)
    endPan = (endPan+180)/360
    endPan = endPan*(np.pi/2)
    lEndPan = np.cos(endPan)
    rEndPan = np.sin(endPan)
    lPan = np.linspace(lStartPan, lEndPan, length*samplerate)
    rPan = np.linspace(rStartPan, rEndPan, length*samplerate)
    y = createSineWave(freq, length, samplerate)
    x[:,0] = lPan*y
    x[:,1] = rPan*y
    return x
    
#outData = createMovingPannedSineWave(1000, length, samplerate, 180, -180) 
#outData = outData + createMovingPannedSineWave(2500, length, samplerate, -180, 100)
outData = createPannedSineWave(200, length, samplerate, -90)
outData = outData + createPannedSineWave(320, length, samplerate, -50)
outData = outData + createPannedSineWave(540, length, samplerate, 50)
outData = outData + createPannedSineWave(1764, length, samplerate, 90)
sf.write(filename, outData, samplerate)