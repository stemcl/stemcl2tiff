//
//  main.c
//  stemcl2tiff
//
//  Created by Manuel Radek on 19.11.16.
//  Copyright © 2016 Manuel Radek. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <tiffio.h>
#include <dirent.h>
#include <string.h>
#include "tiff.h"

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t detector;
    float value;
} located_intensity;

char *extension;

static int parse_ext(const struct dirent *dir)
{
    if(!dir)
        return 0;
    
    if(dir->d_type == DT_REG) { /* only deal with regular file */
        const char *ext = strrchr(dir->d_name,'.');
        if((!ext) || (ext == dir->d_name))
            return 0;
        else {
            if(strcmp(ext, extension) == 0)
                return 1;
        }
    }
    
    return 0;
}

int main(int argc, const char * argv[]) {

    if (argc <= 4) {
        fprintf(stdout, "Usage: %s <pixel: x> <pixel: y> <input extension> <num. detectors> <merge?>\n", argv[0]);
    }
    int nx_out = atoi(argv[1]);
    int ny_out = atoi(argv[2]);
    extension = (char *)argv[3];
    int num_detect = atoi(argv[4]);
    
    int b_merged = 1;
    if (argc > 5) {
        b_merged = 0;
    }
    
    tiff_image image;
    image.width = nx_out;
    image.height = ny_out;
    
    float *input_data = malloc(nx_out * ny_out * (num_detect) * sizeof(float));
    
    float min[num_detect];
    float max[num_detect];
    for (int i = 0; i < num_detect; i++) {
        min[i] = 1000.0;
        max[i] = 0.0;
    }
    
    struct dirent **namelist;
    int n;
    
    n = scandir(".", &namelist, parse_ext, alphasort);
    if (n < 0) {
        perror("scandir");
        return 1;
    }
    else {
        while (n--) {
            
            FILE *input_file = fopen(namelist[n]->d_name, "rb");
                        
            if (b_merged == 1) {
                for (int i = 0; i < nx_out*ny_out*num_detect; i++) {
                    input_data[i] = 0.0;
                }
                
                for (int i = 0; i < num_detect; i++) {
                    min[i] = 1000.0;
                    max[i] = 0.0;
                }
            }
                
            if (input_file != NULL) {
                for (int i = 0; i < nx_out*ny_out*num_detect; i++) {
                    located_intensity intensity;
                    fread(&intensity,sizeof(located_intensity),1,input_file);
                    int index = intensity.detector*nx_out*ny_out + intensity.x*ny_out + intensity.y;
                    input_data[index] = intensity.value;
                    
                    if (min[intensity.detector] > input_data[index] && input_data[index] > 0.0) min[intensity.detector] = input_data[index];
                    if (max[intensity.detector] < input_data[index]) max[intensity.detector] = input_data[index];
                    
                }
            }
            
            fclose(input_file);

            if (b_merged == 1) {
                for (int idetect = 0; idetect < num_detect; idetect++) {
                    
                    float *output_data = malloc(nx_out*ny_out*sizeof(float));
                    
                    for (int x = 0; x < nx_out; x++) {
                        for (int y = 0; y < ny_out; y++) {
                            float prr = input_data[idetect*nx_out*ny_out + x * ny_out + y];
                            float value = prr;
                            input_data[idetect*nx_out*ny_out + x * ny_out + y] = value;
                            output_data[x*ny_out + y] = value;
                        }
                    }
                    image.data = output_data;
                    char *filename_tiff = malloc(1024);
                    sprintf(filename_tiff, "%s-detector-%d.tiff", namelist[n]->d_name, idetect+1);
                    tiff_write((char *)filename_tiff,image, min[idetect], max[idetect]);
                    free(output_data);
                }
            }

        }
        
        if (b_merged == 0) {
            
            for (int idetect = 0; idetect < num_detect; idetect++) {
                
                float *output_data = malloc(nx_out*ny_out*sizeof(float));
                
                for (int x = 0; x < nx_out; x++) {
                    for (int y = 0; y < ny_out; y++) {
                        float prr = input_data[idetect*nx_out*ny_out + x * ny_out + y];
                        float value = prr;
                        input_data[idetect*nx_out*ny_out + x * ny_out + y] = value;
                        output_data[x*ny_out + y] = value;
                    }
                }
                image.data = output_data;
                char *filename_tiff = malloc(1024);
                sprintf(filename_tiff, "merged-detector-%d.tiff", idetect+1);
                tiff_write((char *)filename_tiff,image, min[idetect], max[idetect]);
                free(output_data);
            }
        }
        
        free(namelist);
    }
}
