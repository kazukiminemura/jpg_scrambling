import argparse
import struct
import numpy as np
import matplotlib.pyplot as plt
import subprocess

# from array import array


def get_dct(filename):
    # run jpeg decoder to generate dct.bin
    process = subprocess.Popen(['./jpeg-9d/jpegtran -d {}'.format(filename)],
                        stdout=subprocess.PIPE, 
                        stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    # stdout, stderr
    print(stdout)

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
        for i in range(0, 512*512):
            stream = file.read(4)
            dct.append(struct.unpack('I', stream)[0])
        for i in range(0, 256*256):
            stream = file.read(4)
            dct_cb.append(struct.unpack('I', stream)[0])
        for i in range(0, 256*256):
            stream = file.read(4)
            dct_cr.append(struct.unpack('I', stream)[0])

    dct_im = np.zeros((512,512))

    blk_i=0
    for m_row in range(0,64):
        for m_col in range(0,64):
            block = np.array(dct[blk_i*64:blk_i*64+64])
            blk_i += 1
            for h in range(0,8):
                for w in range(0,8):
                    dct_im[m_row*8+h, m_col*8 +w]=block[h*8+w]

    plt.imshow(dct_im)
    plt.waitforbuttonpress()
    plt.close()

    return dct


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('filename', type=str,
                        help='jpeg image')
    # parser.add_argument('--sum', dest='accumulate', action='store_const',
    #                     const=sum, default=max,
    #                     help='sum the integers (default: find the max)')

    args = parser.parse_args()
    print('input file: ' + args.filename)
    
    get_dct(args.filename)


    


    # print(dct)
    # print(dct_im)


    # processed = dct_im

    # dct_out=[]
    # for ind in range(0,32*32): # MCU index
    #     m_row = int(ind / 32)
    #     m_col = int(ind % 32)    
    #     for b_row in range(0,2):
    #         for b_col in range(0,2):
    #             blk_i += 1
    #             for h in range(0,8):
    #                 for w in range(0,8):
    #                     dct_out.append(int(processed[b_row*8+m_row*16 +h, b_col*8+m_col*16 +w]))

    # with open("out.bin", mode='wb') as file:
    #     for coef in dct_out:
    #         file.write(struct.pack('I', coef))
    #     for coef in dct_cb:
    #         file.write(struct.pack('I', coef))
    #     for coef in dct_cr:
    #         file.write(struct.pack('I', coef))