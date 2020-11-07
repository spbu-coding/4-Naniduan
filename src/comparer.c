#include "lib_converter.c"

int main(int argc, char **argv){
    char *name1 = argv[1], *name2 = argv[2];
    FILE *file1, *file2;

    file1 =fopen(name1, "rb");
    file2 =fopen(name2, "rb");

    struct MY_BMP_HEADER bmp_header1, bmp_header2;
    int error = get_header(file1, &bmp_header1);
    if (error!=0){
        fprintf(stderr, "comparer can't read the header of the first file\n");
        return -1;
    }
    error = get_header(file2, &bmp_header2);
    if (error!=0){
        fprintf(stderr, "comparer can't read the header of the second file\n");
        return -1;
    }

    error = compare_headers(&bmp_header1, &bmp_header2);
    if (error!=0) return -1;

    if (bmp_header1.number_of_colors == 0) bmp_header1.number_of_colors = 256;
    const unsigned int palette_size = bmp_header1.number_of_colors;
    error = compare_pixels(file1, file2, &bmp_header1, &palette_size);
    if (error!=0) return -1;

    return 0;
}