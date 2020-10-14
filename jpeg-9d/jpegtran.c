#include <stdio.h>
#include <stdlib.h>

#include "jpeglib.h"             /* Common decls for cjpeg/djpeg applications */
#include "transupp.h"            /* Support routines for jpegtran */
#include "cdjpeg.h"

static const char * progname;

void usage (void)
/* complain about bad command line */
{
fprintf(stderr, "usage: %s [switches] ", progname);
fprintf(stderr, "[inputfile]\n");

fprintf(stderr, "Switches (names may be abbreviated):\n");
fprintf(stderr, "  -d  decode and extract dct from source file\n");
fprintf(stderr, "  -e  dct.bin encode dct into file\n");

exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
  // parser
  progname = argv[0];
  int process_optiopn=-1;
  FILE *fp;
  char *input_file;
  char *output_file;

  int argn=0;
  char * arg;
  for (argn = 1; argn < argc; argn++) {
      arg = argv[argn];

      if (*arg != '-') {
      /* Not a switch, must be a file name argument */
        break;			/* else done parsing switches */
      }
      arg++;			/* advance past switch marker character */

      if (keymatch(arg, "d", 1)) {
          printf("decode mode\n");
          process_optiopn=0;
          if (++argn >= argc)       /* advance to next argument */
            usage();
          output_file = argv[argn];
      } else if (keymatch(arg, "e", 1)) {
          printf("encode mode\n");
          process_optiopn=1;
          if (++argn >= argc)       /* advance to next argument */
            usage();
          input_file = argv[argn];
      } else if (keymatch(arg, "o", 1)) {
          if (++argn >= argc)       /* advance to next argument */
            usage();
          output_file = argv[argn];
      }else
      {
          usage();
      }
  }

    if (argn < argc - 2) {
        fprintf(stderr, "%s: only one input file\n", progname);
        usage();
    }

    if (argn < argc) {
        if ((fp = fopen(argv[argn], READ_BINARY)) == NULL) {
        fprintf(stderr, "%s: can't open %s\n", progname, argv[argn]);
        exit(EXIT_FAILURE);
        }
    }
    printf("opened jpg image\n");

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


    /* Specify data source for decompression */
    jpeg_stdio_src(&srcinfo, fp);
    // printf("pass jpef_stdio_src\n");

    /* Enable saving of extra markers that we want to copy */
    jcopy_markers_setup(&srcinfo, JCOPYOPT_ALL);
    // printf("pass jcopy_markers_setup\n");

    /* Read file header */
    (void) jpeg_read_header(&srcinfo, TRUE);
    // printf("pass jpeg_read_header\n");

    jtransform_request_workspace(&srcinfo, &transformoption);
    src_coef_arrays = jpeg_read_coefficients(&srcinfo);
    jpeg_copy_critical_parameters(&srcinfo, &dstinfo);
    // printf("pass jpeg_read_coefficients\n");

    // added functions
    if (process_optiopn==-1)
    {
        printf("nothing has done\n");
    }

    if (process_optiopn==0)
    {
    size_t block_row_size;
    JBLOCKARRAY coef_buffers[MAX_COMPONENTS];
    JBLOCKARRAY row_ptrs[MAX_COMPONENTS];

        //Allocate DCT array buffers
        for (JDIMENSION compnum=0; compnum<srcinfo.num_components; compnum++)
        {
            coef_buffers[compnum] = (&dstinfo)->mem->alloc_barray(
            (j_common_ptr) &dstinfo, 
            JPOOL_IMAGE, 
            srcinfo.comp_info[compnum].width_in_blocks,
            srcinfo.comp_info[compnum].height_in_blocks);
        }

        // printf("got dct buffer\n");
        FILE *fh = fopen (output_file, "wb");
        //For each component,
        for (JDIMENSION compnum=0; compnum<srcinfo.num_components; compnum++)
        {
            // printf("%d\n", compnum);
            block_row_size = (size_t) sizeof(JCOEF)*DCTSIZE2*srcinfo.comp_info[compnum].width_in_blocks;
            //...iterate over rows,
            // printf("%d\n", srcinfo.comp_info[compnum].height_in_blocks);
            // printf("%d\n", srcinfo.comp_info[compnum].width_in_blocks);
            for (JDIMENSION rownum=0; rownum<srcinfo.comp_info[compnum].height_in_blocks; rownum++)
            {
                row_ptrs[compnum] = (&dstinfo)->mem->access_virt_barray((j_common_ptr) &dstinfo, src_coef_arrays[compnum], rownum, (JDIMENSION) 1, FALSE);
                //...and for each block in a row,
                for (JDIMENSION blocknum=0; blocknum<srcinfo.comp_info[compnum].width_in_blocks; blocknum++)
                {
                    //...iterate over DCT coefficients
                    for (JDIMENSION i=0; i<DCTSIZE2; i++)
                    {
                        if (fh != NULL) // 32, 1, 1, 0, 0, 0
                        {
                            fwrite (&row_ptrs[compnum][0][blocknum][i], sizeof(JDIMENSION), 1, fh);   
                        }
                    }
                }
            }
        }
        fclose (fh);
        printf("wrote to DCT values into %s\n", output_file);
    }

    if (process_optiopn==1)
    {
        size_t block_row_size;
        JBLOCKARRAY coef_buffers[MAX_COMPONENTS];
        JBLOCKARRAY row_ptrs[MAX_COMPONENTS];
        for (JDIMENSION compnum=0; compnum<srcinfo.num_components; compnum++)
        {
            coef_buffers[compnum] = (&dstinfo)->mem->alloc_barray(
            (j_common_ptr) &dstinfo, 
            JPOOL_IMAGE, 
            srcinfo.comp_info[compnum].width_in_blocks,
            srcinfo.comp_info[compnum].height_in_blocks);
        }
        
        FILE *fh = fopen (input_file, "rb");
        for (JDIMENSION compnum=0; compnum<srcinfo.num_components; compnum++)
        {
            block_row_size = (size_t) sizeof(JCOEF)*DCTSIZE2*srcinfo.comp_info[compnum].width_in_blocks;
            //...iterate over rows,
            for (JDIMENSION rownum=0; rownum<srcinfo.comp_info[compnum].height_in_blocks; rownum++)
            {
                for (JDIMENSION blocknum=0; blocknum<srcinfo.comp_info[compnum].width_in_blocks; blocknum++)
                {
                //...and for each blockprognameCT coefficientss
                    for (JDIMENSION i=0; i<DCTSIZE2; i++)
                    {
                        if (fh != NULL) {
                            int dummy = fread (&coef_buffers[compnum][rownum][blocknum][i], sizeof (JDIMENSION), 1, fh);
                        }
                    }
                }
            }
        }
        fclose(fh);
        printf("loaded DCT values from %s\n", input_file);

        //Save the changes
        for (JDIMENSION compnum=0; compnum<srcinfo.num_components; compnum++)
        {
          block_row_size = (size_t) sizeof(JCOEF)*DCTSIZE2 * srcinfo.comp_info[compnum].width_in_blocks;
        //   printf("%d, %lu, %lu\n",srcinfo.comp_info[compnum].width_in_blocks, sizeof(JCOEF)*DCTSIZE2, block_row_size);
          for (JDIMENSION rownum=0; rownum < srcinfo.comp_info[compnum].height_in_blocks; rownum++)
          {
              row_ptrs[compnum] = (&dstinfo)->mem->access_virt_barray((j_common_ptr) &dstinfo, src_coef_arrays[compnum], rownum, (JDIMENSION) 1, TRUE);
              memcpy(row_ptrs[compnum][0][0], coef_buffers[compnum][rownum][0], block_row_size);
          }
        }

      /* ..when done with DCT, do this: */
      dst_coef_arrays = jtransform_adjust_parameters(&srcinfo, &dstinfo, src_coef_arrays, &transformoption);
    //   dst_coef_arrays = src_coef_arrays;

      //And write everything back
      FILE *outfile_pointer;
      outfile_pointer = fopen(output_file, "wb");
      /* Specify data destination for compression */
      jpeg_stdio_dest(&dstinfo, outfile_pointer);
      /* Start compressor (note no image data is actually written here) */
      jpeg_write_coefficients(&dstinfo, dst_coef_arrays);
      /* Copy to the output file any extra markers that we want to preserve */
      jcopy_markers_execute(&srcinfo, &dstinfo, JCOPYOPT_ALL);
      jpeg_finish_compress(&dstinfo);
      jpeg_destroy_compress(&dstinfo);
      fclose(outfile_pointer);
    }

    (void) jpeg_finish_decompress(&srcinfo);
    jpeg_destroy_decompress(&srcinfo);
    fclose(fp);
}