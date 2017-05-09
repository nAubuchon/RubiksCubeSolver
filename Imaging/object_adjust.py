import cv2
import numpy as np

cap1 = cv2.VideoCapture(0)

while(True):
    ret, img = cap1.read()

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

    ### method 2 ###
    i1 = img[:, :, 2]
    i1 = i1 - cv2.erode(i1, None)
    template = cv2.imread('images/template.png')[:, :, 2]
    template = template - cv2.erode(template, None)
    ccnorm = cv2.matchTemplate(i1, template, cv2.TM_CCORR_NORMED)

    # print ccnorm.max()
    loc = np.where(ccnorm == ccnorm.max())
    threshold = 0.4
    th, tw = template.shape[:2]
    for pt in zip(*loc[::-1]):
        if ccnorm[pt[::-1]] < threshold:
            continue
        cv2.rectangle(img, pt, (pt[0] + tw, pt[1] + th), (0, 0, 255), 2)


    # Display the resulting frame
    cv2.imshow('result', img)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything done, release the capture
cap1.release()
cv2.destroyAllWindows()