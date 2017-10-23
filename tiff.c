//
//  stemcl2tiff.c
//  stemcl2tiff
//
//  Created by Manuel Radek on 19.11.16.
//  Copyright © 2016 Manuel Radek. All rights reserved.
//

#include <tiffio.h>
#include <string.h>
#include <stdlib.h>
#include "tiff.h"

void tiff_write(char *filename, tiff_image image, double min, double max) {
    
    TIFF *tiff = TIFFOpen(filename, "w");
    
    TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, image.width);
    TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, image.height);
    TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 32);
    TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP);
    TIFFSetField(tiff, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_SEPARATE);
    TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tiff, image.height));
    
    tsize_t scanline_size = TIFFScanlineSize(tiff);
    float *buffer = _TIFFmalloc(scanline_size);
    
    for (uint32_t row = 0; row < image.height; row++) {
        memcpy(buffer + 0 * image.width, &image.data[image.width * row], scanline_size);
        TIFFWriteScanline(tiff, buffer, row, 0);
    }
    
    _TIFFfree(buffer);
    TIFFClose(tiff);
}

tiff_image tiff_read(char *filename) {
    
    tiff_image image;
    TIFF *tiff = TIFFOpen(filename, "r");
    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &image.height);
    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &image.width);
    tsize_t scanline_size = TIFFScanlineSize(tiff);
    float *buffer = _TIFFmalloc(scanline_size);
    float *data = malloc(sizeof(float) * image.width * image.height);
    image.data = data;
    
    for (uint32_t row = 0; row < image.height; row++) {
        TIFFReadScanline(tiff, buffer, row, 0);
        memcpy(&image.data[image.width * row], buffer + 0 * image.width, scanline_size/2);
    }
    
    _TIFFfree(buffer);
    TIFFClose(tiff);
    return image;
}
