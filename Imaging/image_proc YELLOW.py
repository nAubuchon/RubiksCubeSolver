import matplotlib.image as mpimg
import numpy as np
import matplotlib.patches as patches
import matplotlib.pyplot as plt

rTint = 1.00
gTint = 1.00
bTint = 1.00

pics = ['1', '2', '3']
locs = [(2, 8, 17, 20), (10, 16, 19, 20), (11, 12, 21, 24)]
arr = np.array([
    (155, 87), (240, 52), (315, 20), (227, 129), (386, 52), (311, 180), (390, 134), (465, 87),          # up
    (120, 154), (191, 195), (268, 251), (127, 242), (274, 339), (135, 320), (198, 365), (269, 419),     # left
    (350, 249), (430, 200), (505, 154), (349, 339), (494, 244), (346, 420), (422, 370), (485, 324)      # front
     ])
yR, yG, yB = 0, 0, 0
yItr = 0
yCntr = 0
grid = 20
rgDiff = 0
dmax = 0
dmin = 0

for item in pics:
    img = np.array(mpimg.imread('images/drag/' + item + '.png'), dtype=np.float)
    fig, imgplot = plt.subplots(1)
    imgplot.imshow(img)

    itr = 1
    for index in arr:
        if itr in locs[yItr]:
            rect = patches.Rectangle(index, grid, grid, linewidth=1, edgecolor='purple', facecolor='none')
            imgplot.add_patch(rect)

            i = index[1]
            j = index[0]
            subimg = np.array(img[i:i + grid, j:j + grid])

            x, y, z = 0, 0, 0
            d = 0
            dnaught = 0

            for a in range(0, grid):
                for b in range(0, grid):
                    x += subimg[a][b][0] * rTint
                    y += subimg[a][b][1] * gTint
                    z += subimg[a][b][2] * bTint
                    dnaught = subimg[a][b][1] -  subimg[a][b][0]
                    d += dnaught
                    if ( dnaught > dmax ):
                        dmax = dnaught
                    elif ( dnaught < dmin ):
                        dmin = dnaught

            x /= grid*grid
            y /= grid*grid
            z /= grid*grid
            d /= grid*grid

            yR += x
            yG += y
            yB += z
            rgDiff += d

            yCntr += 1

        itr += 1

    yItr += 1

    # plt.show()

yR /= yCntr
yG /= yCntr
yB /= yCntr
rgDiff /= yCntr

if yR < 0.005:
    yR = 0
if yG < 0.005:
    yG = 0
if yB < 0.005:
    yB = 0

print 'Yellow avgR: ', yR
print 'Yellow avgG: ', yG
print 'Yellow avgB: ', yB
print 'Green - Red max: ', dmax
print 'Green - Red min: ', dmin
print 'Green - Red avg: ', rgDiff