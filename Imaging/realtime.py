import cv2
import numpy as np
import matplotlib.image as mpimg
import matplotlib.pyplot as plt
import kociemba as kc
import serial

pics = ['1', '2', '3', '4']

for item in pics:
    img = cv2.imread('images/image' + item + '.png')

    # ### method 1 ###
    # img = cv2.cvtColor(frame1, cv2.COLOR_BGR2GRAY)
    #
    # template = cv2.imread('images/template2.png', 0)
    # w = template.shape[0]
    # h = template.shape[1]
    #
    # res = cv2.matchTemplate(img, template, cv2.TM_CCORR)
    # min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
    # top_left = max_loc
    # bottom_right = (top_left[0] + w, top_left[1] + h)
    #
    # cv2.rectangle(img, top_left, bottom_right, 255, 2)

    #create subplot
    fig, imgplot = plt.subplots(1)

    ### method 2 ###
    img2 = img[:, :, 2]
    img2 = img2 - cv2.erode(img2, None)
    template = cv2.imread('images/template3.png')[:, :, 2]
    template = template - cv2.erode(template, None)

    ccnorm = cv2.matchTemplate(img2, template, cv2.TM_CCORR_NORMED)
    # print ccnorm.max()
    loc = np.where(ccnorm == ccnorm.max())
    threshold = 0.4
    th, tw = template.shape[:2]
    for pt in zip(*loc[::-1]):
        if ccnorm[pt[::-1]] < threshold:
            continue
        cv2.rectangle(img, pt, (pt[0] + tw, pt[1] + th), (0, 0, 255), 2)
        # print 'pt[0]: ', pt[0]
        # print 'pt[1]: ', pt[1]
        # print 'tw: ', tw
        # print 'th: ', th

    subimg = img[pt[1]:pt[1]+th, pt[0]:pt[0]+tw]
    subimg = 255 - subimg

    # Display the resulting frame
    # cv2.imshow('result', subimg)
    imgplot.imshow(subimg)
    plt.show()
