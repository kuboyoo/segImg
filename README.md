# segImg
Superpixel-wise Region Annotation Tool for Image Segmentation. (My bachelor's thesis in 2018)

![png](https://github.com/kuboyoo/segImg/blob/master/assets/segImg.png)

## Requirements
* g++
* OpenCV (with contrib)
* Qt5

## Usage
### Linux
```
cd src
qmake 
make -j4
./segImg
```

### Mac OS
```cd src```
add final line in `segImg.pro`
```
QMAKE_MACOSX_DEPLOYMENT_TARGET=10.9
```
```
qmake
make -j4
./segImg.app/Contents/MacOS/segImg
```

## Lisense
This software includes the work that is distributed in the Apache License 2.0
