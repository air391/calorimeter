import numpy as np
import sys
import matplotlib.pyplot as plt
import subprocess
nCell = 40
path = "B4a/build/B4.csv"

def run_cmd(cmd, f):
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
    output, error = p.communicate()
    return f(output.strip())

if __name__ == "__main__":
    note_num = run_cmd(f"cat {path} | grep '#' | wc -l", int)
    num = run_cmd(f"cat {path} | grep -v '#' | wc -l", int)
    data = np.loadtxt(path, dtype=np.float64, delimiter=',', skiprows=note_num)
    egap = data[:,0]
    labs = data[:,1]
    lgap = data[:,2]
    lsen = data[:,3]
    eSen = [data[:,i+4] for i in range(nCell*nCell)]

    eAbs = [data[:,i+nCell*nCell+3] for i in range(nCell*nCell)]

    eSen2d = np.reshape(eSen, [nCell,nCell,num])
    eAbs2d = np.reshape(eAbs, [nCell,nCell,num])
    fig, axs = plt.subplots(2, 2, figsize=(10, 8))
    h1 = axs[0,0].imshow(np.sum(eAbs2d, axis=2), cmap='plasma', interpolation='nearest')
    h2 = axs[0,1].imshow(np.sum(eSen2d, axis=2), cmap='plasma', interpolation='nearest')
    h3 = axs[1,0].imshow(eAbs2d[:,:,998], cmap='plasma', interpolation='nearest')
    h4 = axs[1,1].imshow(eSen2d[:,:,0], cmap='plasma', interpolation='nearest')
    # plt.tight_layout()
    cbar = fig.colorbar(h4, ax=axs, pad=0.1, fraction=0.03)
    plt.show()