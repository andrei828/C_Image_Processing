# Image processing tool and file encryption for transfering results

[![Build Status](https://travis-ci.org/joemccann/dillinger.svg?branch=master)](https://github.com/andrei828/C_Image_Processing) ![Build Status](https://img.shields.io/github/repo-size/andrei828/C_Image_Processing) ![Build Status](https://img.shields.io/github/last-commit/andrei828/C_Image_Processing)

# Introduction
This repo is dedicated to image processing using plain C. The images used are mostly **.bmp** files. The main features can be found inside the **Encryption & Image recognition** directory. There a two main files.        
- **crypting_decrypting.c** is used to encrypt and decrypt images (turning from their oringinal state to a pixelated one by swapping pixels and changing their values slightly using an encryption key)
- **template_matching.c** is used for image recognition using the same algorithm found in face-detecting cameras. To test this file, we used a .bmp template for each digit then showed a matrix with multiple handwritten digits. The file detects with 91% accuracy the value of the digits.

Besides the two main features, there are also a few files for experimenting with image processing. 
| File name | Description |
| ------ | ------ |
| lower_brightness.c | Lowers the brightness of an image given as input. |
| negative_image.c | Inverts the values of all pixels. |
| picture_in_picture.c | Overwrites a picture on top of another picture. |
# Demo
> All images can be found inside the **Encryption & Image recognition/images** directory.

#### For image encryption/decryption
![Original](https://github.com/andrei828/C_Image_Processing/blob/master/Encryption%20%26%20Image%20recognition/images/peppers/peppers.bmp =200x200)
