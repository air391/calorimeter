import numpy as np
import matplotlib.pyplot as plt
path = "B4a/build/output_senDep"
header = "G4WT1 > --- StepAction: edep in sen "

with open(path, "r") as f:
    edep = (float(e[len(header):]) for e in f.readlines())
    data = np.fromiter(edep, dtype=np.float64)
plt.hist(data, bins=100)
plt.show()