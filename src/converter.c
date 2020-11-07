#include <stdio.h>
#include "lib_converter.c"
#include "qdbmp.h"

#define MY_realisation converter

int QDBMP_realisation(char *input_name, char *output_name){
    UCHAR	r, g, b;
    UINT	width, height;
    UINT	x, y;
    BMP*	bmp;

    bmp = BMP_ReadFile( input_name );
    BMP_CHECK_ERROR( stdout, -1 );

    /* Get image's dimensions */
    width = BMP_GetWidth( bmp );
    height = BMP_GetHeight( bmp );

    /* Iterate through all the image's pixels */
    for ( x = 0 ; x < width ; ++x )
    {
        for ( y = 0 ; y < height ; ++y )
        {
            /* Get pixel's RGB values */
            BMP_GetPixelRGB( bmp, x, y, &r, &g, &b );

            /* Invert RGB values */
            BMP_SetPixelRGB( bmp, x, y, 255 - r, 255 - g, 255 - b );
        }
    }

    /* Save result */
    BMP_WriteFile( bmp, output_name );
    BMP_CHECK_ERROR( stdout, -2 );


    /* Free all memory allocated for the image */
    BMP_Free( bmp );

    return 0;
}

int main(int argc, char **argv){
    struct MY_BMP_HEADER bmp_header;
    char *realisation = argv[1], *input_name = argv[2],  *output_name = argv[3], *mine = "--mine";

    if (strncmp(realisation, mine, strlen(mine)) == 0){
        FILE *input_file;
        input_file = fopen(input_name, "rb+");
        int error = get_header(input_file, &bmp_header);
        if (error != 0) return  error;

        fseek(input_file, 0, SEEK_SET);
        FILE *output_file;
        output_file = fopen(output_name, "wb+");
        error = MY_realisation(input_file, output_file, &bmp_header);
        if (error != 0) return  error;
    }
    else{
        int error = QDBMP_realisation(input_name, output_name);
        if (error != 0) return QDBMP_ERROR;
    }

    return 0;
}