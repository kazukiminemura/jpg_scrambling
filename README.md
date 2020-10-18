# jpg_scrambling
JPEG image scrambling without expansion in bitstream size


# compile fixed jpg lib
cd jpeg-9d  
mmkdir build  
config --prefix <path/to/build>  
make -j  
make install

# DCT coefficient extraction
[dct_y, dct_cb, dct_cr, blocksizes] = get_dct(args.filename)  
fig = plt.figure()  
fig, (ax1, ax2, ax3) = plt.subplots(1, 3)  
ax1.imshow(dct_y)  
ax2.imshow(dct_cb)  
ax3.imshow(dct_cr)  

![Extracted DCT](https://github.com/kazukiminemura/jpg_scrambling/blob/master/lena-dct.png "DCT values of lenna.jpg")


# Scrambling
DC Shuffling  
Block Shuffling  
![Extracted DCT](https://github.com/kazukiminemura/jpg_scrambling/blob/master/scrambled.png "Scrambled of lenna.jpg")