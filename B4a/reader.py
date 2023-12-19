import numpy as np
import time
import subprocess
import functools
import matplotlib.pyplot as plt
path = "./build/B4_1_1000.csv"
nofCells = 40
calorSizeXY = 1.01 # cm
def extract(path):
    energy = int(path.split('_')[1])
    num = int(path.split('_')[2].split('.')[0])
    return energy, num

def reader_root(path):
    import uproot
    with uproot.open(path) as f:
        tree = f['B4']
        tree.show()
        start_time = time.time()
        keys = tree.keys()
        print("keys:",time.time() - start_time)
        egap = tree.arrays(keys[0])
        labs = tree.arrays(keys[1])
        lgap = tree.arrays(keys[2])
        lsen = tree.arrays(keys[3])
        print("arrays:",time.time() - start_time)
        eSen = tree.arrays([keys[i+4] for i in range(nofCells*nofCells)])
        eAbs = tree.arrays([keys[i+nofCells*nofCells+4] for i in range(nofCells*nofCells)])
        print("arrays",time.time() - start_time)
        print(keys[4])
        print(keys[nofCells*nofCells+4])
        return egap, labs, lgap, lsen, eSen, eAbs
def run_cmd(cmd, f):
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
    output, error = p.communicate()
    return f(output.strip())
def reader_csv(path):
    note_num = run_cmd(f"cat {path} | grep '#' | wc -l", int)
    data = np.loadtxt(path, dtype=np.float64, delimiter=',', skiprows=note_num)
    egap = data[:,0]
    labs = data[:,1]
    lgap = data[:,2]
    lsen = data[:,3]
    eSen = data[:, 4:4+nofCells*nofCells]
    eAbs = data[:, 4+nofCells*nofCells:]
    return egap, labs, lgap, lsen, eSen, eAbs

def pixel_pos(i,j):
    # return pixel center position with unit cm
    x = (i - nofCells/2. + 0.5) * calorSizeXY
    y = (j - nofCells/2. + 0.5) * calorSizeXY
    return np.array([x, y])
@functools.lru_cache(maxsize=2)
def pixel_pos_list():
    return np.array([pixel_pos(i,j) for i in range(nofCells) for j in range(nofCells)])

def distance(pos1, pos2):
    return np.linalg.norm(pos1-pos2)
def find_shower_pixel(idx_max):
    # find pixels with distance less than 3.5cm with idx_max of pixel
    max_pos = pixel_pos_list()[idx_max]
    shower_pixels = np.array([i for i in range(nofCells*nofCells) if distance(pixel_pos_list()[i], max_pos) < 3.5])
    return shower_pixels
if __name__ == "__main__":
    energy, num = extract(path)
    egap, labs, lgap, lsen, eSen, eAbs = reader_csv(path)
    energys = np.zeros(num)
    pos = np.zeros((num,2))
    for i in range(eSen.shape[0]):
        eSen_i = eSen[i]
        idx_max = np.argmax(eSen_i)
        shower_pixels = find_shower_pixel(idx_max)
        shower_energy = np.sum(eSen_i[shower_pixels])
        shower_pos = np.average(pixel_pos_list()[shower_pixels], axis=0, weights=eSen_i[shower_pixels])
        energys[i] = shower_energy
        pos[i,:] = shower_pos
    fig, axs = plt.subplots(2, 2, figsize=(10, 10))
    fig.suptitle(f"energy:{energy}MeV, num:{num}")
    axs[0,0].set_title("energy histogram")
    axs[0,1].set_title("position scatter")
    axs[1,0].set_title("energy deposit of last one")
    axs[1,1].set_title("shower range of last one")
    h1 = axs[0,0].hist(energys,bins=50)
    h2 = axs[0,1].scatter(pos[:,0], pos[:,1])
    eSen2d = np.reshape(eSen, (nofCells,nofCells,num))
    h3 = axs[1,0].imshow(np.reshape(eSen[-1,:], (nofCells,nofCells)), cmap='plasma', interpolation='nearest')
    eSen[-1,:][shower_pixels] = 100
    h4 = axs[1,1].imshow(np.reshape(eSen[-1,:],(nofCells,nofCells)), cmap='plasma', interpolation='nearest')
    plt.show()