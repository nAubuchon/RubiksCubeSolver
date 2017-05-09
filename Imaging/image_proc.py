import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np
import kociemba as kc
import serial

### For Testing ###
# import matplotlib.patches as patches
# faces = ['U%d', 'R%d', 'F%d', 'D%d', 'L%d', 'B%d']  #, 'F%d', 'R%d', 'D%d']
# fItr = 0

#States of the cube
faces = ""
solved = "UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB"

#Array of instructions to be transmitted
instr = np.zeros(40, dtype=np.uint8)

#location, face, array and centerpiece iterators
lItr = 0
aItr = 0
cItr = 0

#picture filenames
pics = ['1', '2']#, '3']

#for skipping centerpieces
centers = "URFDLB"

#locations in images of cube faces
locs1 = np.array([
    (315, 20), (386, 52), (465, 87), (240, 52), (390, 134), (155, 87), (227, 129), (311, 180),          # up
    (350, 249), (430, 200), (505, 154), (349, 339), (494, 244), (346, 420), (422, 370), (485, 324),     # front
    (120, 154), (191, 195), (268, 251), (127, 242), (274, 339), (135, 320), (198, 365), (269, 419)      # left
     ])
locs2 = np.array([
    (457, 364), (380, 404), (306, 440), (387, 320), (231, 402), (305, 272), (225, 322), (148, 364),     # down
    (345, 32), (415, 84), (479, 131), (347, 114), (488, 210), (346, 199), (424, 252), (497, 295),       # back
    (131, 133), (194, 89), (269, 37), (123, 213), (267, 115), (113, 300), (188, 256), (265, 205)        # left
    ])
locs3 = np.array([
    (131, 133), (194, 89), (269, 37), (123, 213), (267, 115), (113, 300), (188, 256), (265, 205),       # front
    (345, 32), (415, 84), (479, 131), (347, 114), (488, 210), (346, 199), (424, 252), (497, 295),       # right
    (148, 364), (225, 322), (305, 272), (231, 402), (387, 320), (306, 440), (380, 404), (457, 364)      # down
    ])

#when switching from one image to the next
locs_arr = [locs1, locs2, locs3]

#sub-image gridsize
grid = 20


#begin processing images
for item in pics:
    #read image
    img = np.array(mpimg.imread('images/drag/' + item + '.png'), dtype=np.float)

    #create subplot
    fig, imgplot = plt.subplots(1)

    ### For Testing ###
    # imgplot.imshow(img)

    #reset location index
    lItr = 1

    #step through locations
    for index in locs_arr[aItr]:
        #create sub-image
        i = index[1]
        j = index[0]
        subimg = np.array(img[i:i + grid, j:j + grid])

        #reset RGB sums
        x, y, z = 0, 0, 0

        #step through pixels of sub-image
        for a in range(0, grid):
            for b in range(0, grid):
                #sum RGB values
                x += subimg[a][b][0]
                y += subimg[a][b][1]
                z += subimg[a][b][2]

        #average RGB values
        x /= grid*grid
        y /= grid*grid
        z /= grid*grid

        #trim negligible values
        if x < 0.005:
            x = 0
        if y < 0.005:
            y = 0
        if z < 0.005:
            z = 0

        ### For Testing ###
        # rect = patches.Rectangle(index, grid, grid, linewidth=1, edgecolor='purple', facecolor='none')
        # imgplot.add_patch(rect)
        # text = plt.text(index[0], index[1] + 20, faces[fItr] % lItr, color='purple', fontsize=14)

        #color vector
        col = [x, y, z]

        #if at a centerpiece, add the next element to the state string (faces)
        if (lItr == 5):
            faces += centers[cItr]
            cItr += 1

        #determine color based on RGB weights
        if (col[0] > 0.50) and (col[1] > 0.60) and (col[2] > 0.55):
            faces += 'U'    #White
        elif (col[0] > 0.6) and (col[1] > 0.75) and (col[2] < 0.3):
            faces += 'D'    #Yellow
        elif (col[0] < 0.05) and (col[1] < 0.05) and (col[2] > 0.1):
            faces += 'R'    #Blue
        elif (col[0] > 0.15) and (col[1] > 0.60) and (col[2] < 0.25):
            faces += 'L'    #Green
        elif (col[0] > 0.25) and (col[1] < 0.05) and (col[2] < 0.05):
            faces += 'F'    #Red
        elif (col[0] > 0.8) and (col[1] > 0.10) and (col[2] < 0.1):
            faces += 'B'    #Orange
        else:
            print 'Cannot determine state, aborting'
            exit()

        #iterate to next location
        lItr += 1

        #if on the last piece of the face, reset and iterate to the next face
        if lItr == 9:
            lItr = 1

            ### For Testing ###
            #fItr += 1

    #iterate
    aItr += 1

    ### For Testing ###
    # imgplot.imshow(img)
    # plt.show()
    # plt.savefig('images/mapping' + item + '.png')

#send state to solving algorithm
result = kc.solve(solved, faces)
print result

#dictionary for instruction bitmasks
instrDict = {'U': 0x02, 'R': 0x04, 'F': 0x08, 'D': 0x10, 'L': 0x20, 'B': 0x40}

#instruction string iterator
i = 0

#instruction byte
j = np.uint8(0)

#instruction buffer
ibuff = np.uint8(0)

#translate instruction string into bitmasks
while (i < len(result)):
    if (not(result[i] == ' ')):
        ibuff = instrDict[result[i]]
        if (result[i+1]=='\''):
            instr[j] = ibuff + 1
            j += 1
            i += 1
        elif (result[i+1]=='2'):
            instr[j] = ibuff
            j += 1
            instr[j] = ibuff
            j += 1
            i += 1
        else:
            instr[j] = ibuff
            j += 1
            i += 1
    i += 1

print instr

# #setup serial to send instructions to mC
# ser = serial.Serial('/dev/cu.usbserial-A100CYN0')
# ser.baudrate = 9600
# ser.parity = 'N'
# ser.stopbits = serial.STOPBITS_ONE
#
# #flush buffer
# ser.flush()
#
# #iterate through array of byte instructions
# # and send over USART
# i = 1
# sbuff = chr(0x7A - instr[0])
# ser.write(sbuff)
# while i < len(instr):
#     #await signal for next instruction
#     if ser.in_waiting==1:
#         sbuff = ser.read(1)
#         ser.flush()
#         #signal recieved
#         if sbuff=='1':
#             #grab instruction
#             sbuff = chr(0x7A - instr[i])
#             #send
#             ser.write(sbuff)
#             #print current instruction byte
#             print sbuff
#             i += 1
