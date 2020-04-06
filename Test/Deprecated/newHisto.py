import numpy as np
import matplotlib.pyplot as plt

nValues = 100
#values = [1, 1.4, 1.8, 2, 2.4, 2.8, 3, 3.5, 4, 4.5, 5, 6, 7, 8, 9, 10]
values = np.zeros(nValues)
for i in range(nValues):
    values[i] = i

maxElement = max(values)
nBins = 10

linBins = np.zeros(nBins)
curvedBins = np.zeros(nBins)

increment = maxElement/nBins

# calculate curved bins
for index in range(len(values)):
    i = ((values[index]/maxElement)**2) * maxElement
    i = i/increment
    if(i == len(curvedBins)):
        i = len(curvedBins)-1
    curvedBins[int(i)] = curvedBins[int(i)]+1

plt.plot(range(len(curvedBins)), curvedBins)
plt.show()

for index in range(len(values)):
    i = values[index]/increment
    linBins[int(i)-1] = linBins[int(i)-1]+1

plt.plot(range(len(linBins)), linBins)
plt.show()

print("Lin Bins")
for i in range(len(linBins)):
    print(linBins[i])