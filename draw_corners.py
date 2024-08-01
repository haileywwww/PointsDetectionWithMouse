import os
import cv2
import glob
import numpy as np
from PIL import Image, ImageEnhance

BaseDir = 'save3mm_0328/'
SaveDir = 'save3mm_0328/coners/'
FileNameWavue = 'wavue_points_0328.txt'
FileNameObbr = 'obbr_points_0328.txt'
w = 7
h = 4
objpoints = []
imgpoints = []


if not os.path.exists(SaveDir):
    os.makedirs(SaveDir)
if not os.path.exists(SaveDir + 'obbr/'):
    os.makedirs(SaveDir + 'obbr/')
if not os.path.exists(SaveDir + 'wavue/'):
    os.makedirs(SaveDir + 'wavue/')

def get_coordinates(fname, h, w):
    image_name = []
    coords = []
    for t in open(fname).read().split():
        if not  'save' in t:
            a, b = t.strip('()').split(',')
            coords.append((int(a), int(b)))
        else:
            image_name.append(t)
    coords = np.array(coords).reshape(-1, h*w, 1, 2)
    return image_name, coords


image_name, corners = get_coordinates(FileNameObbr, h, w)
for i in range(len(image_name)):
    print(image_name[i][:-4] + '_calib.png')
    print(SaveDir + 'obbr/' + image_name[i].split('/')[-1])
    corners_one = corners[i].astype('f4')
    image = cv2.imread(image_name[i][:-4] + '_calib.png')
    cv2.drawChessboardCorners(image, (h,w), corners_one, True)
    cv2.namedWindow('findCorners', cv2.WINDOW_NORMAL)
    cv2.imshow('findCorners', image)
    cv2.imwrite(SaveDir + 'obbr/' + image_name[i].split('/')[-1], image)
    cv2.waitKey(200)


image_name, corners = get_coordinates(FileNameWavue, h, w)
for i in range(len(image_name)):
    print(image_name[i])
    print(SaveDir + 'wavue/' + image_name[i].split('/')[-1])
    corners_one = corners[i].astype('f4')
    image1 = np.fromfile(image_name[i],'uint8').reshape(1280,1080)
    cv2.imwrite(BaseDir + image_name[i].split('/')[-1][:-4] + '.png', image1)
    image = cv2.imread(BaseDir + image_name[i].split('/')[-1][:-4] + '.png')
    cv2.drawChessboardCorners(image, (h,w), corners_one, True)
    cv2.namedWindow('findCorners', cv2.WINDOW_NORMAL)
    cv2.imshow('findCorners', image)
    cv2.imwrite(SaveDir + 'wavue/' + image_name[i].split('/')[-1][:-4] + '.png', image)
    cv2.waitKey(200)

