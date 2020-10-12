#include <stdio.h>
#include <stdlib.h>

#include "jpeglib.h"             /* Common decls for cjpeg/djpeg applications */
#include "transupp.h"            /* Support routines for jpegtran */


int main()
{

struct jpeg_decompress_struct srcinfo;
struct jpeg_compress_struct dstinfo;
struct jpeg_error_mgr jsrcerr, jdsterr;

static jpeg_transform_info transformoption; /* image transformation options */
transformoption.transform = JXFORM_NONE;
transformoption.trim = FALSE;
transformoption.force_grayscale = FALSE;

jvirt_barray_ptr * src_coef_arrays;
jvirt_barray_ptr * dst_coef_arrays;

/* Initialize the JPEG decompression object with default error handling. */
srcinfo.err = jpeg_std_error(&jsrcerr);
jpeg_create_decompress(&srcinfo);
/* Initialize the JPEG compression object with default error handling. */
dstinfo.err = jpeg_std_error(&jdsterr);
jpeg_create_compress(&dstinfo);

FILE *fp;
char *filePath="./lena.jpg";
// char *filePath;

if((fp = fopen(filePath, "rb")) == NULL) {
    //Throw an error
} else {
    //Continue
}

/* Specify data source for decompression */
jpeg_stdio_src(&srcinfo, fp);

/* Enable saving of extra markers that we want to copy */
jcopy_markers_setup(&srcinfo, JCOPYOPT_ALL);

/* Read file header */
(void) jpeg_read_header(&srcinfo, TRUE);

jtransform_request_workspace(&srcinfo, &transformoption);
src_coef_arrays = jpeg_read_coefficients(&srcinfo);
jpeg_copy_critical_parameters(&srcinfo, &dstinfo);

/* Do your DCT shenanigans here on src_coef_arrays like this (I've moved it into a separate function): */


// moveDCTAround(&srcinfo, &dstinfo, 0, src_coef_arrays);

    size_t block_row_size;
    JBLOCKARRAY coef_buffers[MAX_COMPONENTS];
    JBLOCKARRAY row_ptrs[MAX_COMPONENTS];
    printf("%d\n",MAX_COMPONENTS);

    //Allocate DCT array buffers
    // printf("%d\n", srcinfo.num_components);
    for (JDIMENSION compnum=0; compnum<srcinfo.num_components; compnum++)
    {
        printf("%d, %d\n", srcinfo.comp_info[compnum].height_in_blocks, srcinfo.comp_info[compnum].width_in_blocks);
      coef_buffers[compnum] = ((dstinfo).mem->alloc_barray)(
        (j_common_ptr) &dstinfo, 
        JPOOL_IMAGE, 
        srcinfo.comp_info[compnum].width_in_blocks,
        srcinfo.comp_info[compnum].height_in_blocks);
    }

    printf("got dct buffer\n");

    FILE *fh = fopen ("dct.bin", "wb");
    //For each component,
    for (JDIMENSION compnum=0; compnum<srcinfo.num_components; compnum++)
    {
        // printf("%d\n", compnum);
        block_row_size = (size_t) sizeof(JCOEF)*DCTSIZE2*srcinfo.comp_info[compnum].width_in_blocks;
        //...iterate over rows,
        for (JDIMENSION rownum=0; rownum<srcinfo.comp_info[compnum].height_in_blocks; rownum++)
        {
            row_ptrs[compnum] = ((dstinfo).mem->access_virt_barray)((j_common_ptr) &dstinfo, src_coef_arrays[compnum], rownum, (JDIMENSION) 1, FALSE);

            // printf("%d, %d, %d\n", rownum, compnum, srcinfo.comp_info[compnum].width_in_blocks);
            //...and for each block in a row,
            for (JDIMENSION blocknum=0; blocknum<srcinfo.comp_info[compnum].width_in_blocks; blocknum++)
            {
                //...iterate over DCT coefficients
                for (JDIMENSION i=0; i<DCTSIZE2; i++)
                {
                        //Manipulate your DCT coefficients here. For instance, the code here inverts the image.
                        // coef_buffers[compnum][rownum][blocknum][i] = row_ptrs[compnum][0][blocknum][i];
                        // printf("%d\n", coef_buffers[compnum][rownum][blocknum][i]);

                    if (fh != NULL) {
                        fwrite (&row_ptrs[compnum][0][blocknum][i], sizeof (JDIMENSION), 1, fh);        
                    }
                }
            }
        }
    }
    fclose (fh);

    // //Save the changes
    // //For each component,
    // for (JDIMENSION compnum=0; compnum<srcinfo.num_components; compnum++)
    // {
    //   block_row_size = (size_t) sizeof(JCOEF)*DCTSIZE2 * srcinfo.comp_info[compnum].width_in_blocks;
    //   for (JDIMENSION rownum=0; rownum < srcinfo.comp_info[compnum].height_in_blocks; rownum++)
    //   {
    //     row_ptrs[compnum] = ((dstinfo).mem->access_virt_barray)((j_common_ptr) &dstinfo, src_coef_arrays[compnum], rownum, (JDIMENSION) 1, TRUE);
    //     memcpy(row_ptrs[compnum][0][0], coef_buffers[compnum][rownum][0], block_row_size);
    //   }
    // }


// /* ..when done with DCT, do this: */
// dst_coef_arrays = jtransform_adjust_parameters(&srcinfo, &dstinfo, src_coef_arrays, &transformoption);
// fclose(fp);
// //And write everything back
// fp = fopen(filePath, "wb");
// /* Specify data destination for compression */
// jpeg_stdio_dest(&dstinfo, fp);
// /* Start compressor (note no image data is actually written here) */
// jpeg_write_coefficients(&dstinfo, dst_coef_arrays);
// /* Copy to the output file any extra markers that we want to preserve */
// jcopy_markers_execute(&srcinfo, &dstinfo, JCOPYOPT_ALL);
// jpeg_finish_compress(&dstinfo);
// jpeg_destroy_compress(&dstinfo);


(void) jpeg_finish_decompress(&srcinfo);
jpeg_destroy_decompress(&srcinfo);

fclose(fp);
}