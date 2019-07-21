# -*- coding: utf-8 -*-
"""
Created on Tue Jul 16 16:05:21 2019

@author: 86134
"""

import cv2
import matplotlib.pyplot as plt

cap = cv2.VideoCapture(0)
ret, frame = cap.read()

plt.imshow(frame)