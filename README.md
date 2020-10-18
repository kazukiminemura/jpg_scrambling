# jpg_scrambling
JPEG image scrambling without expansion in bitstream size


# compile fixed jpg lib
cd jpeg-9d  
mmkdir build  
config --prefix <path/to/build>  
make -j  
make install

# DCT coefficient extraction
![Extracted DCT](https://github.com/kazukiminemura/jpg_scrambling/blob/master/lena-dct.png "DCT values of lenna.jpg")
