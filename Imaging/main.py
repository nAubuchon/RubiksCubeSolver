import matplotlib.patches as patches
import matplotlib.pyplot as plt
import cv2

cap1 = cv2.VideoCapture(0)
overlay = cv2.imread('images/OVRL2.png')

while True:
    # Capture frame-by-frame
    ret, frame = cap1.read()
    if cv2.waitKey(1) & 0xFF == ord('y'):
        cv2.imwrite('images/image.png', frame)

    # Display the resulting frame
    newFrame = cv2.add(frame,overlay)
    cv2.imshow('frame', newFrame)
    # cv2.imshow('frame', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything done, release the capture
cap1.release()
overlay.release()
cv2.destroyAllWindows()