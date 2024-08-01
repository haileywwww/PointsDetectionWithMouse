import os
import cv2
import glob
import numpy as np
from PIL import Image, ImageEnhance

RIGHT = 1
Obbr_Camera = 1
BaseDir = 'save0605_calib/right/'
SaveDir = 'save0605_calib/right/data/'
FileNameWavue = BaseDir + 'wavue_points_0605_right.txt'
w = 7
h = 4
objpoints = []
imgpoints = []

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



if not os.path.exists(SaveDir):
    os.makedirs(SaveDir)
if not os.path.exists(SaveDir + "camera1/board_ir"):
    os.makedirs(SaveDir + "camera1/board_ir")
if not os.path.exists(SaveDir + "camera1/board_depth"):
    os.makedirs(SaveDir + "camera1/board_depth")
if not os.path.exists(SaveDir + "camera2/board_ir"):
    os.makedirs(SaveDir + "camera2/board_ir")
if not os.path.exists(SaveDir + "camera1/board_depth_filled"):
    os.makedirs(SaveDir + "camera1/board_depth_filled")

image_name, corners = get_coordinates(FileNameWavue, h, w)
for i in range(len(image_name)):
    num_index = image_name[i].split('/')[-1][:-4]
    fname_wavue = image_name[i]
    fname_ir = image_name[i].split('/')[0] + '/right/Ir_' + num_index + '_calib.png'
    fname_depth = image_name[i].split('/')[0] + '/right/Raw_Depth_' + num_index + '_0.txt'

    # print(num_index)
    print(fname_wavue)
    print(fname_ir)
    print(fname_depth)

    fname_wavue_name = num_index + '.png'
    fname_ir_name = num_index + '.png'
    fname_depth_name = num_index + '.txt'

    # process camera2
    save_name = SaveDir + 'camera2/board_ir/' + fname_wavue_name
    print('save wavue: ', save_name)
    # image = np.fromfile(fname_wavue, 'uint8').reshape(1280, 1080)
    image = cv2.imread(fname_wavue)
    cv2.imwrite(save_name, image)

    #process camera1 ir
    save_name = SaveDir + 'camera1/board_ir/' + fname_ir_name
    print('save ir: ', save_name)
    image = cv2.imread(fname_ir)
    cv2.imwrite(save_name, image)

    #process camera1 depth
    save_name = SaveDir + 'camera1/board_depth/' + fname_depth_name
    print('save depth: ', save_name)
    if Obbr_Camera == 1:
        if RIGHT == 1:
            img = np.loadtxt(fname_depth).reshape(480, 640)
            img = cv2.rotate(img, cv2.ROTATE_90_COUNTERCLOCKWISE)
            img = cv2.resize(img, [1080, 1440])
            img = img[80:-80, :]
            img = cv2.flip(img, 1)
        else:
            img = np.loadtxt(fname_depth).reshape(480, 640)
            img = cv2.resize(img, [1706, 1280])
            img = img[:, 313:-313]
            img = cv2.flip(img, 1)
    else:
        img = np.loadtxt(fname_depth).reshape(576, 640)
        img = img[:, 32:-32]
        img = cv2.resize(img, [1280, 1280])
        img = img[:, 100:-100]
    np.savetxt(save_name, img)
    


