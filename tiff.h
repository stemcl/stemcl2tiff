//
//  stemcl2tiff.h
//  stemcl2tiff
//
//  Created by Manuel Radek on 19.11.16.
//  Copyright © 2016 Manuel Radek. All rights reserved.
//

#include <stdint.h>

typedef struct {
    uint32_t width;
    uint32_t height;
    float *data;
} tiff_image;

extern const char *tiff_version;

/**
 * @brief
 * Write complex data as IEEE floating point TIFF image. Real and imaginary
 * parts will stored side by side in a single image. The stored image will
 * have the same height and twice the width of the complex_tiff_image struct
 * passed to this function. Real part will be on the left side of the image.
 */
void tiff_write(char *filename, tiff_image image, double min, double max);

/**
 * @brief
 * Read a complex valued TIFF image. The real and imaginary components of the
 * returned structure are guaranteed to be pointers to different positions of
 * the same memory object.
 */
tiff_image tiff_read(char *filename);
