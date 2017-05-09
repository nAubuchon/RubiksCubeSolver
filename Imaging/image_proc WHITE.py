import matplotlib.image as mpimg
import numpy as np
import matplotlib.patches as patches
import matplotlib.pyplot as plt

rTint = 1.00
gTint = 1.00
bTint = 1.00

pics = ['1', '2', '3']
locs = [(9, 22, 23), (1, 2, 7, 11, 21), (3, 4, 5, 13, 18, 19)]
arr = np.array([
    (155, 87), (240, 52), (315, 20), (227, 129), (386, 52), (311, 180), (390, 134), (465, 87),          # up
    (120, 154), (191, 195), (268, 251), (127, 242), (274, 339), (135, 320), (198, 365), (269, 419),     # left
    (350, 249), (430, 200), (505, 154), (349, 339), (494, 244), (346, 420), (422, 370), (485, 324)      # front
     ])
wR, wG, wB = 0, 0, 0
# s = 0
wItr = 0
wCntr = 0
grid = 20

for item in pics:
    img = np.array(mpimg.imread('images/drag/' + item + '.png'), dtype=np.float)
    fig, imgplot = plt.subplots(1)
    imgplot.imshow(img)

    itr = 1
    for index in arr:
        if itr in locs[wItr]:
            rect = patches.Rectangle(index, grid, grid, linewidth=1, edgecolor='purple', facecolor='none')
            imgplot.add_patch(rect)

            i = index[1]
            j = index[0]
            subimg = np.array(img[i:i + grid, j:j + grid])

            x, y, z = 0, 0, 0
            # sum = 0

            for a in range(0, grid):
                for b in range(0, grid):
                    x += subimg[a][b][0] * rTint
                    y += subimg[a][b][1] * gTint
                    z += subimg[a][b][2] * bTint
                    # sum += (subimg[a][b][0] + subimg[a][b][1] + subimg[a][b][2])/3

            x /= grid*grid
            y /= grid*grid
            z /= grid*grid
            # sum /= grid*grid

            wR += x
            wG += y
            wB += z
            # s += sum

            wCntr += 1

        itr += 1

    wItr += 1

    plt.show()

wR /= wCntr
wG /= wCntr
wB /= wCntr
s /= wCntr

if wR < 0.005:
    wR = 0
if wG < 0.005:
    wG = 0
if wB < 0.005:
    wB = 0

print 'White avgR: ', wR
print 'White avgG: ', wG
print 'White avgB: ', wB
print 'Avg ratio: ', s

