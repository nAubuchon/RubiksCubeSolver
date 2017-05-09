import matplotlib.image as mpimg
import numpy as np
import matplotlib.patches as patches
import matplotlib.pyplot as plt

rTint = 1.07
gTint = 1.00
bTint = 1.00

pics = ['1', '2', '3']
locs = [(1, 6, 10), (3, 5, 12, 15, 22), (7, 8, 14)]
arr = np.array([
    (155, 87), (240, 52), (315, 20), (227, 129), (386, 52), (311, 180), (390, 134), (465, 87),          # up
    (120, 154), (191, 195), (268, 251), (127, 242), (274, 339), (135, 320), (198, 365), (269, 419),     # left
    (350, 249), (430, 200), (505, 154), (349, 339), (494, 244), (346, 420), (422, 370), (485, 324)      # front
     ])
oR, oG, oB = 0, 0, 0
oItr = 0
oCntr = 0
grid = 20

for item in pics:
    img = np.array(mpimg.imread('images/drag/' + item + '.png'), dtype=np.float)
    fig, imgplot = plt.subplots(1)
    imgplot.imshow(img)

    itr = 1
    for index in arr:
        if itr in locs[oItr]:
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

            oR += x
            oG += y
            oB += z

            oCntr += 1

        itr += 1

    oItr += 1

    plt.show()

oR /= oCntr
oG /= oCntr
oB /= oCntr

if oR < 0.005:
    oR = 0
if oG < 0.005:
    oG = 0
if oB < 0.005:
    oB = 0

print 'Orange avgR: ', oR
print 'Orange avgG: ', oG
print 'Orange avgB: ', oB

