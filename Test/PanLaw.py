import matplotlib.pyplot as plt
import numpy as np

x = np.arange(0, 1, 0.01)

s = x
c = x

for i in range(len(x)):
    s[i] = np.sin(x[i])
    c[i] = np.cos(x[i])

#plt.plot(range(len(s)), s, c)
plt.plot(c)
plt.show()
print("finito")