import argparse
import struct
import numpy as np
import matplotlib.pyplot as plt
import subprocess
import os
import re

# from array import array


def get_dct(filename):
    # run jpeg decoder to generate dct.bin
    command = os.path.join('./jpeg-9d','jpegtran') + " -d " + filename
    process = subprocess.check_output(command, shell=True,  stderr=subprocess.STDOUT)
    index = process.decode().find(": ")
    imagearray = process.decode()[index+1:]

    # print(imagearray.split())
    [height_y, weidth_y, height_cb, weidth_cb, height_cr, weidth_cr, \
        block_h_y, block_w_y, block_h_cb, block_w_cb, block_h_cr, block_w_cr,
        ] \
        = map(int,imagearray.split())
    # print([height_y, weidth_y, height_cb, weidth_cb, height_cr, weidth_cr, \
    #     block_h_y, block_w_y, block_h_cb, block_w_cb, block_h_cr, block_w_cr])

    blocksizes = [block_h_y, block_w_y, block_h_cb, block_w_cb, block_h_cr, block_w_cr]

    '''
    Charactor, Byte-order
    @,         nativesys
    <,         little endian
    >,         big endian

    Format, C-type,         Python-type, Size[byte]
    c,      char,           byte,        1
    b,      signed char,    integer,     1
    B,      unsigned char,  integer,     1
    h,      short,          integer,     2
    H,      unsigned short, integer,     2
    i,      int,            integer,     4
    I,      unsigned int,   integer,     4
    f,      float,          float,       4
    d,      double,         float,       8
    '''

    dct = []
    dct_cb = []
    dct_cr = []
    with open('./dct.bin', mode='rb') as file: # b is important -> binary
        for i in range(0, height_y*weidth_y):
            stream = file.read(4)
            dct.append(struct.unpack('I', stream)[0])
        for i in range(0, height_cb*weidth_cb):
            stream = file.read(4)
            dct_cb.append(struct.unpack('I', stream)[0])
        for i in range(0, height_cr*weidth_cr):
            stream = file.read(4)
            dct_cr.append(struct.unpack('I', stream)[0])

    dct_2d_y = np.zeros((height_y,weidth_y))
    dct_2d_cb = np.zeros((height_cb,weidth_cb))
    dct_2d_cr = np.zeros((height_cr,weidth_cr))

    # print(len(dct_cb))

    blk_i=0
    for m_row in range(0,block_h_y):
        for m_col in range(0,block_w_y):
            block = np.array(dct[blk_i*block_h_y:blk_i*block_w_y+block_h_y])
            blk_i += 1
            for h in range(0,8):
                for w in range(0,8):
                    dct_2d_y[m_row*8+h, m_col*8 +w]=block[h*8+w]

    blk_i=0
    for m_row in range(0,block_h_cb):
        for m_col in range(0,block_w_cb):
            # print(m_row, m_col)
            block = np.array(dct_cb[blk_i*64:blk_i*64+64])
            blk_i+=1
            for h in range(0,8):
                for w in range(0,8):
                    # pass
                    dct_2d_cb[m_row*8+h, m_col*8 +w]=block[h*8+w]

    blk_i=0
    for m_row in range(0,block_h_cr):
        for m_col in range(0,block_w_cr):
            block = np.array(dct_cr[blk_i*64:blk_i*64+64])
            blk_i+=1
            for h in range(0,8):
                for w in range(0,8):
                    dct_2d_cr[m_row*8+h, m_col*8 +w]=block[h*8+w]

    return dct_2d_y, dct_2d_cb, dct_2d_cr, blocksizes 

def reencode_dct(filename, outfilename, dct_y, dct_cb, dct_cr, blocksizes):
    # print(blocksizes)
    [block_h_y, block_w_y, block_h_cb, block_w_cb, block_h_cr, block_w_cr] = blocksizes

    dct_y_out=[]
    for m_row in range(0,block_h_y):
        for m_col in range(0,block_w_y):
            for h in range(0,8):
                for w in range(0,8):
                    dct_y_out.append(int(dct_y[m_row*8 + h, m_col*8 + w]))

    dct_cb_out=[]
    for m_row in range(0,block_h_cb):
        for m_col in range(0,block_w_cb):
            for h in range(0,8):
                for w in range(0,8):
                    dct_cb_out.append(int(dct_cb[m_row*8 + h, m_col*8 + w]))

    dct_cr_out=[]
    for m_row in range(0,block_h_cr):
        for m_col in range(0,block_w_cr):
            for h in range(0,8):
                for w in range(0,8):
                    dct_cr_out.append(int(dct_cr[m_row*8 + h, m_col*8 + w]))

    with open("out.bin", mode='wb') as file:
        for coef in dct_y_out:
            file.write(struct.pack('I', coef))
        for coef in dct_cb_out:
            file.write(struct.pack('I', coef))
        for coef in dct_cr_out:
            file.write(struct.pack('I', coef))

    # run jpeg decoder to generate dct.bin
    command = os.path.join('./jpeg-9d','jpegtran') + " -e " + filename + " -o " + outfilename + " -b out.bin"
    subprocess.check_output(command, shell=True,  stderr=subprocess.STDOUT)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('filename', type=str,
                        help='jpeg image')
    parser.add_argument('outfilename', type=str,
                        help='jpeg image')

    args = parser.parse_args()
    # print('input file: ' + args.filename)
    # print('output file: ' + args.outfilename)

    # get DCT    
    [dct_y, dct_cb, dct_cr, blocksizes] = get_dct(args.filename)

    fig = plt.figure()
    fig, (ax1, ax2, ax3) = plt.subplots(1, 3)
    ax1.imshow(dct_y)
    ax2.imshow(dct_cb)
    ax3.imshow(dct_cr)
    plt.waitforbuttonpress()
    plt.close()

    # Re-encode
    reencode_dct(args.filename, args.outfilename, dct_y, dct_cb, dct_cr, blocksizes)




    
