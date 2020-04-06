'''
import MultiLevelThreshold

MLT = MultiLevelThreshold.MultiLevelThreshold(2, 1024, 100, 2)
x = MLT.getNumberOfExtractedSources()
print(x)
'''

import numpy as np
import soundfile as sf

for block in sf.blocks('TestSignal-m75-m10-30-90.wav', blocksize=1024, overlap=0, dtype='float32'):
    x = block[:,0]
    print("c")
print("finito")