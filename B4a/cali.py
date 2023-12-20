from utils import reader_csv, get_exp_info, gaus_fit
import numpy as np
import matplotlib.pyplot as plt
import utils
Energy = [1,2,5,10,50] # GeV
path = [f"./build/B4_{e}_1000.csv" for e in Energy]
edep = np.zeros(len(Energy), dtype=np.float64)
res = np.zeros(len(Energy), dtype=np.float64)
if __name__ == '__main__':
    for i,(e, p) in enumerate(zip(Energy, path)):
        egap, labs, lgap, lsen, eSen, eAbs = reader_csv(p)
        energys, pos = get_exp_info(eSen)
        center, resolution = gaus_fit(energys)
        edep[i], res[i] = center*1e-3, resolution
    p = np.polyfit(edep,Energy, 1)
    plt.plot(edep,Energy, 'o', label="data")
    plt.plot(edep, np.polyval(p, edep), label=f"fit:{p[0]:.2f}x+{p[1]:.2f}")
    plt.legend()
    plt.show()