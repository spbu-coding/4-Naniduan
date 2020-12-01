#include <stdio.h>
#include <string.h>
#include "converter.h"

int read_uint(unsigned int* x, FILE* file) {
    unsigned char input[4];
    if(fread(input, 4, 1, file) != 1) {
        if(feof(file) != 0) {
            fprintf(stderr, "the file has unexpectedly ended\n");
        } else {
            fprintf(stderr, "for whatever reason the converter can't read the needed data\n");
        }
    return 0;
    }
    *x = (input[3] << 24 | input[2] << 16 | input[1] << 8 | input[0]);
    return 1;
}

int read_ushort(unsigned short* x, FILE* file) {
    unsigned char input[2];
    if(fread(input, 2, 1, file) != 1) {
        if(feof(file) != 0) {
            fprintf(stderr, "the file has unexpectedly ended\n");
        } else {
            fprintf(stderr, "for whatever reason the converter can't read the needed data\n");
        }
        return 0;
    }
    *x = (input[1] << 8 | input[0]);
    return 1;
}

int read_int(int* x, FILE* file) {
    char input[4];
    if(fread(input, 4, 1, file) != 1) {
        if(feof(file) != 0) {
            fprintf(stderr, "the file has unexpectedly ended\n");
        } else {
            fprintf(stderr, "for whatever reason the converter can't read the needed data\n");
        }
        return 0;
    }
    *x = (input[3] << 24 | input[2] << 16 | input[1] << 8 | input[0]);
    return 1;
}

int read_char(unsigned char* x, FILE* file) {
    unsigned char input[1];
    if(fread(input, 1, 1, file) != 1) {
        if(feof(file) != 0) {
            fprintf(stderr, "the file has unexpectedly ended\n");
        } else {
            fprintf(stderr, "for whatever reason the converter can't read the needed data\n");
        }
        return 0;
    }
    *x = input[0];
    return 1;
}

int write_uint(unsigned int x, FILE* file) {
    unsigned char output[4];
    output[3] = (unsigned char)((x & 0xff000000) >> 24);
    output[2] = (unsigned char)((x & 0x00ff0000) >> 16);
    output[1] = (unsigned char)((x & 0x0000ff00) >> 8);
    output[0] = (unsigned char)((x & 0x000000ff) >> 0);

    return (file && fwrite(output, 4, 1, file) == 1);
}

int write_int(int x, FILE* file) {
    char output[4];
    output[3] = (char)((x & 0xff000000) >> 24);
    output[2] = (char)((x & 0x00ff0000) >> 16);
    output[1] = (char)((x & 0x0000ff00) >> 8);
    output[0] = (char)((x & 0x000000ff) >> 0);

    return (file && fwrite(output, 4, 1, file) == 1);
}

int write_ushort(unsigned short x, FILE* file) {
    char output[2];
    output[1] = (unsigned char)((x & 0xff00) >> 8);
    output[0] = (unsigned char)((x & 0x00ff) >> 0);
    return (file && fwrite(output, 2, 1, file) == 1);
}

int write_char(unsigned char x, FILE* file) {
    char output[1];
    output[0] = (unsigned char)x;
    return (file && fwrite(output, 1, 1, file) == 1);
}

bool check_type(unsigned short type) {
    char possible_values[6][2] = {"BM", "BA", "CI", "CP", "IC", "PT"};
    for(unsigned int i = 0; i < 6; i++) {
        if(memcmp(&type, possible_values, 2) == 0) return true;
    }
    return false;
}

int get_header(FILE* file, struct MY_BMP_HEADER *bmp_header) {
    bool something_is_null = false;

    unsigned int calculated_size;
    fseek(file, 0, SEEK_END);
    calculated_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    if(calculated_size < 26) {
        fprintf(stderr, "the file is too small\n");
        return STRUCTURE_ERROR;
    }
    if(read_ushort(&bmp_header->type, file) == 0) something_is_null = true;
    if(!check_type(bmp_header->type)) {
        fprintf(stderr, "the signature says it isn't .bmp file\n");
        return WRONG_FORMAT;
    }

    if(read_uint(&bmp_header->size, file) == 0) something_is_null = true;
    bmp_header->size = calculated_size;

    unsigned int reserved;
    if(read_uint(&reserved, file) == 0) something_is_null = true;
    if(reserved != 0) {
        fprintf(stderr, "the file structure doesn't match the standard\n");
        return STRUCTURE_ERROR;
    }

    if(read_uint(&bmp_header->pixel_array_offset, file) == 0) something_is_null = true;

    unsigned bmp_version;
    if(read_uint(&bmp_version, file) == 0) something_is_null = true;
    if(bmp_version != 40) {
        fprintf(stderr, "the converter doesn't support this version of .bmp\n");
        return WRONG_FORMAT;
    }

    if(read_int(&bmp_header->width, file) == 0) something_is_null = true;
    if(read_int(&bmp_header->height, file) == 0) something_is_null = true;

    unsigned short number_of_color_planes;
    if(read_ushort(&number_of_color_planes, file) == 0) something_is_null = true;
    if(number_of_color_planes != 1) {
        fprintf(stderr, "the file structure doesn't match the standard\n");
        return STRUCTURE_ERROR;
    }

    if(read_ushort(&bmp_header->bits_per_pixel, file) == 0) something_is_null = true;
    if(bmp_header->bits_per_pixel != 8 && bmp_header->bits_per_pixel != 24) {
        fprintf(stderr, "the converter only supports 8bpp and 24bpp .bmp files\n");
        return WRONG_FORMAT;
    }

    unsigned int compression_method;
    if(read_uint(&compression_method, file) == 0) something_is_null = true;
    if(compression_method != 0) {
        fprintf(stderr, "the converter doesn't support compressed .bmp files\n");
        return WRONG_FORMAT;
    }

    if(read_uint(&bmp_header->image_size, file) == 0) something_is_null = true;
    if(bmp_header->image_size != 0) {
        unsigned int calculated_image_size = bmp_header->size - bmp_header->pixel_array_offset;
        if(calculated_image_size != bmp_header->image_size) {
            fprintf(stderr, "the size of the image isn't equal to the size mentioned in the header\n");
            return STRUCTURE_ERROR;
        }
    }

    if(read_uint(&bmp_header->horizontal_resolution, file) == 0) something_is_null = true;
    if(read_uint(&bmp_header->vertical_resolution, file) == 0) something_is_null = true;
    if(read_uint(&bmp_header->number_of_colors, file) == 0) something_is_null = true;
    if(read_uint(&bmp_header->number_of_important_colors, file) == 0) something_is_null = true;

    if (something_is_null){
        fprintf(stderr, "converter can't read the header\n");
        return STRUCTURE_ERROR;
    }
    return 0;
}

int converter(FILE *input_file, FILE *output_file, struct MY_BMP_HEADER *bmp_header){
    unsigned int palette_start = 54, palette_end = bmp_header->pixel_array_offset;
    unsigned char byte=0;

    while(ftell(input_file) < bmp_header->size){
        if (read_char(&byte, input_file) == 0){
            fprintf(stderr, "the converter can't read the image while converting\n");
            return STRUCTURE_ERROR;
        }
        if (bmp_header->bits_per_pixel == 8 &&
        ftell(input_file)>palette_start &&
        ftell(input_file)<palette_end &&
        (ftell(input_file)-palette_start)%4 != 0) {
            byte = ~byte;
        }

        else if (bmp_header->bits_per_pixel == 24 &&
                ftell(input_file)>bmp_header->pixel_array_offset){
            byte = ~byte;
        }

        write_char(byte, output_file);
    }
    return 0;
}

int compare_headers(struct MY_BMP_HEADER *header1, struct MY_BMP_HEADER *header2){
    if (header1->width != header2->width){
        fprintf(stderr, "the pictures have different widths");
        return -1;
    }
    if (abs(header1->height) != abs(header2->height)) {
        fprintf(stderr, "the pictures have different heights");
        return -1;
    }
    if (header1->bits_per_pixel != header2->bits_per_pixel){
        fprintf(stderr, "the pictures are coded differently");
        return -1;
    }
    if ((header1->number_of_colors != header2->number_of_colors) && header1->bits_per_pixel == 8){
        fprintf(stderr, "the number of colors in palette is different");
        return -1;
    }
    return 0;
}



int compare_pixels(FILE *file1, FILE *file2, struct MY_BMP_HEADER *header, const unsigned int *palette_size){
    unsigned int count_of_pixels=0;
    if (header->height < 0){
        header->height = abs(header->height);
        fprintf(stderr, "the height of the image is negative\n");
    }


    if (header->bits_per_pixel == 8){
        unsigned int palette1[256];
        if (fread(palette1, sizeof(unsigned int), *palette_size, file1) != *palette_size){
            fprintf(stderr, "comparer can't read the palette of the first image\n");
            return -1;
        }
        unsigned int palette2[256];
        if (fread(palette2, sizeof(unsigned int), *palette_size, file2) != *palette_size){
            fprintf(stderr, "comparer can't read the palette of the second image\n");
            return -1;
        }

        unsigned char pixel1 = 0, pixel2 = 0;
        for (int i = 0; i<header->height; i++){
            for(int j = 0; j<header->width; j++){
                if (fread(&pixel1, 1, 1, file1) != 1){
                    fprintf(stderr, "comparer can't read the pixels of the first image\n");
                    return -1;
                }
                if (fread(&pixel2, 1, 1, file2) != 1){
                    fprintf(stderr, "comparer can't read the pixels of the second image\n");
                    return -1;
                }
                if (palette1[pixel1] != palette2[pixel2]){
                    fprintf(stderr,"%d %d\n", j, i);
                    count_of_pixels++;
                }
                if (count_of_pixels >=100) break;
            }
            if (count_of_pixels >=100) break;
        }
    }

    else{
        char pixel1[3], pixel2[3];
        for (int i = 0; i<header->height; i++) {
            for (int j = 0; j < header->width; j++) {
                if (fread(pixel1, 1, 3, file1) != 3){
                    fprintf(stderr, "comparer can't read the pixels of the first image\n");
                    return -1;
                }
                if (fread(pixel2, 1, 3, file2) != 3){
                    fprintf(stderr, "comparer can't read the pixels of the second image\n");
                    return -1;
                }

                int rgb1 = (0x00 << 24 | pixel1[2] << 16 | pixel1[1] << 8 | pixel1[0]);
                int rgb2 = (0x00 << 24 | pixel2[2] << 16 | pixel2[1] << 8 | pixel2[0]);

                if (rgb1!=rgb2){
                    fprintf(stderr,"%d %d\n", j, i);
                    count_of_pixels++;
                }
                if (count_of_pixels >=100) break;
            }
            if (count_of_pixels >=100) break;
        }
    }
    return 0;
}