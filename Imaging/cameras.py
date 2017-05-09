import numpy as np
import cv2

cap1 = cv2.VideoCapture(0)
# cap2 = cv2.VideoCapture(1)
# cap3 = cv2.VideoCapture(2)
# cap4 = cv2.VideoCapture(3)

while(True):
    # Capture frame-by-frame
    ret, frame1 = cap1.read()
    # ret, frame2 = cap2.read()
    # ret, frame3 = cap3.read()
    # ret, frame4 = cap4.read()
    if cv2.waitKey(1) & 0xFF == ord('y'):
        cv2.imwrite('images/image1.png',frame1)
        # cv2.imwrite('images/image2.png', frame2)
        # cv2.imwrite('images/image3.png', frame3)
        # cv2.imwrite('images/image4.png', frame4)
    # Display the resulting frame
    #cam1 = cv2.add(frame,overlay)
    #cv2.imshow('frame',cam1)
    cv2.imshow('frame1', frame1)
    # cv2.imshow('frame2', frame2)
    # cv2.imshow('frame3', frame3)
    # cv2.imshow('frame4', frame4)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything done, release the capture
cap1.release()
# cap2.release()
# cap3.release()
# cap4.release()
cv2.destroyAllWindows()