import cv2
import numpy as np
cap = cv2.VideoCapture("./0001-0150.mp4")

ret, frame1 = cap.read()
prvs = cv2.cvtColor(frame1,cv2.COLOR_BGR2GRAY)
hsv = np.zeros_like(frame1)
hsv[...,1] = 255

frame = 1
while(1):
    frame = frame+1
    ret, frame2 = cap.read()
    ret, frame2 = cap.read()
    next = cv2.cvtColor(frame2,cv2.COLOR_BGR2GRAY)

    flow = cv2.calcOpticalFlowFarneback(prvs,next, 0.5, 3, 15, 3, 5, 1.2, 0)

    # mag, ang = cv2.cartToPolar(flow[...,0], flow[...,1])

    rgb = frame2.copy()
    step = 50
    for x in xrange(step, flow.shape[0], step):
        for y in xrange(step, flow.shape[1], step):
            cv2.arrowedLine(rgb, (y, x), (y + int(10*flow[x,y,1]), x + int(10*flow[x,y,0])), (100,100,255), 4)

    # hsv[...,0] = ang*180/np.pi/2
    # hsv[...,2] = cv2.normalize(mag,None,0,255,cv2.NORM_MINMAX)
    # rgb = cv2.cvtColor(hsv,cv2.COLOR_HSV2BGR)

    cv2.imshow('frame2',rgb)
    k = cv2.waitKey(30) & 0xff
    if k == 27:
        break
    # elif k == ord('s'):
    cv2.imwrite('opticalflow-bullet-' + str(frame) + '.png',rgb)
    prvs = next

cap.release()
cv2.destroyAllWindows()
