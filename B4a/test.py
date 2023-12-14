import numpy as np
import itertools
import matplotlib.pyplot as plt
nCell = 40
path = "B4a/build/B4.csv"
data = np.loadtxt(path, dtype=np.float64, delimiter=',', skiprows=38496)
egap = data[:,0]
labs = data[:,1]
lgap = data[:,2]
lsen = data[:,3]
eSen = [data[:,i+4] for i in range(nCell*nCell)]

eAbs = [data[:,i+nCell*nCell+3] for i in range(nCell*nCell)]

eSen2d = np.zeros((nCell,nCell),dtype=np.float64)
for i, j in itertools.product(range(nCell), range(nCell)):
    eSen2d[i,j] = eSen[nCell*i+j][0]
plt.imshow(eSen2d, cmap='viridis', interpolation='nearest')
plt.colorbar()
plt.show()