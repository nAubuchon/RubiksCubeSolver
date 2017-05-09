import matplotlib.image as mpimg
import numpy as np
import matplotlib.patches as patches
import matplotlib.pyplot as plt

rTint = 1.00
gTint = 1.00
bTint = 1.00

pics = ['1', '2', '3']
locs = [(5, 11, 12, 16, 19), (4, 9, 18), (2, 10, 22)]
arr = np.array([
    (155, 87), (240, 52), (315, 20), (227, 129), (386, 52), (311, 180), (390, 134), (465, 87),          # up
    (120, 154), (191, 195), (268, 251), (127, 242), (274, 339), (135, 320), (198, 365), (269, 419),     # left
    (350, 249), (430, 200), (505, 154), (349, 339), (494, 244), (346, 420), (422, 370), (485, 324)      # front
     ])
bR, bG, bB = 0, 0, 0
bItr = 0
bCntr = 0
grid = 20

for item in pics:
    img = np.array(mpimg.imread('images/drag/' + item + '.png'), dtype=np.float)
    fig, imgplot = plt.subplots(1)
    imgplot.imshow(img)

    itr = 1
    for index in arr:
        if itr in locs[bItr]:
            rect = patches.Rectangle(index, grid, grid, linewidth=1, edgecolor='purple', facecolor='none')
            imgplot.add_patch(rect)

            i = index[1]
            j = index[0]
            subimg = np.array(img[i:i + grid, j:j + grid])

            x, y, z = 0, 0, 0

            for a in range(0, grid):
                for b in range(0, grid):
                    x += subimg[a][b][0] * rTint
                    y += subimg[a][b][1] * gTint
                    z += subimg[a][b][2] * bTint

            x /= grid*grid
            y /= grid*grid
            z /= grid*grid

            bR += x
            bG += y
            bB += z

            bCntr += 1

        itr += 1

    bItr += 1

    plt.show()

bR /= bCntr
bG /= bCntr
bB /= bCntr

if bR < 0.005:
    bR = 0
if bG < 0.005:
    bG = 0
if bB < 0.005:
    bB = 0

print 'Blue avgR: ', bR
print 'Blue avgG: ', bG
print 'Blue avgB: ', bB

