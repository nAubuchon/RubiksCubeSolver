import cv2

## Transforms ##
img1 = cv2.imread('images/image1.png')
img2 = cv2.imread('images/image2.png')
img3 = cv2.imread('images/image3.png')
img4 = cv2.imread('images/image4.png')

# lap1 = cv2.Laplacian(img1, cv2.CV_64F)
# lap2 = cv2.Laplacian(img2, cv2.CV_64F)
# lap3 = cv2.Laplacian(img3, cv2.CV_64F)
# lap4 = cv2.Laplacian(img4, cv2.CV_64F)

# sobx1 = cv2.Sobel(img1, cv2.CV_64F, 1, 0, ksize=5)
# sobx2 = cv2.Sobel(img2, cv2.CV_64F, 1, 0, ksize=5)
# sobx3 = cv2.Sobel(img3, cv2.CV_64F, 1, 0, ksize=5)
# sobx4 = cv2.Sobel(img4, cv2.CV_64F, 1, 0, ksize=5)

# soby1 = cv2.Sobel(img1, cv2.CV_64F, 0, 1, ksize=5)
# soby2 = cv2.Sobel(img2, cv2.CV_64F, 0, 1, ksize=5)
# soby3 = cv2.Sobel(img3, cv2.CV_64F, 0, 1, ksize=5)
# soby4 = cv2.Sobel(img4, cv2.CV_64F, 0, 1, ksize=5)

edges1 = cv2.Canny(img1, 100, 150)
edges2 = cv2.Canny(img2, 100, 150)
edges3 = cv2.Canny(img3, 100, 150)
edges4 = cv2.Canny(img4, 100, 150)

## Display ##
cv2.imshow('img1', img1)
cv2.imshow('img2', img2)
cv2.imshow('img3', img3)
cv2.imshow('img4', img4)

# cv2.imshow('lap1', lap1)
# cv2.imshow('lap2', lap2)
# cv2.imshow('lap3', lap3)
# cv2.imshow('lap4', lap4)

# cv2.imshow('sobx1', sobx1)
# cv2.imshow('sobx2', sobx2)
# cv2.imshow('sobx3', sobx3)
# cv2.imshow('sobx4', sobx4)

# cv2.imshow('soby1', soby1)
# cv2.imshow('soby2', soby2)
# cv2.imshow('soby3', soby3)
# cv2.imshow('soby4', soby4)

cv2.imshow('edges1', edges1)
cv2.imshow('edges2', edges2)
cv2.imshow('edges3', edges3)
cv2.imshow('edges4', edges4)

# cv2.imwrite('images/edges1.png', edges1)
# cv2.imwrite('images/edges2.png', edges2)
# cv2.imwrite('images/edges3.png', edges3)
# cv2.imwrite('images/edges4.png', edges4)

while True:
    # if cv2.waitKey(1) & 0xFF == ord('y'):
    #     cv2.imwrite('images/edges1.png', frame1)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()