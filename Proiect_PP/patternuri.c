#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

typedef struct Window {
    unsigned int X_coord, Y_coord;
} Window;

typedef struct Pixel {
    unsigned char R, G, B;
} Pixel;

typedef struct BMP_info {
    unsigned int num_bytes, width, height;
    unsigned char * header;
} BMP_info;

BMP_info * get_bitmap_data(FILE * tmp);
void recongnize_patterns(int num_of_arguments, ...);
Pixel ** load_image(FILE * tmp, BMP_info * bitmap_data);
Window * calculate_correlation(Pixel ** main_image, Pixel ** template_image, BMP_info * main_image_data, BMP_info * template_image_bitmap, double precision);

unsigned char check_file_error_null(FILE * tmp);
unsigned char check_memory_allocation(const void * data);

int main()
{
    recongnize_patterns(4, "test.bmp", "cifra0.bmp", "cifra1.bmp", "cifra2.bmp");
}

Window * calculate_correlation(Pixel ** main_image, Pixel ** template_image, BMP_info * main_image_data, BMP_info * template_image_bitmap, double precision)
{
    
}

void recongnize_patterns(int num_of_arguments, ...)
{
    va_list argument_list;
    va_start(argument_list, num_of_arguments);

    // first argument is the bitmap where template matching happens
    FILE * main_bitmap = fopen(va_arg(argument_list, const char *), "rb");
    if (check_file_error_null(main_bitmap)) return;

    // next argument are the names of the template files
    FILE * templates_bitmap[num_of_arguments - 1];
    for (int i = 0; i < num_of_arguments - 1; i++)
    {
        templates_bitmap[i] = fopen(va_arg(argument_list, const char *), "rb");
        if (check_file_error_null(templates_bitmap[i]) & 1) return;
    }

    // create matrix of data and file information for bitmap image and templates
    BMP_info * main_bitmap_data = get_bitmap_data(main_bitmap);
    BMP_info * templates_bitmap_data[num_of_arguments - 1];
    for (int i = 0; i < num_of_arguments - 1; i++)
        templates_bitmap_data[i] = get_bitmap_data(templates_bitmap[i]);
    
    Pixel ** main_image = load_image(main_bitmap, main_bitmap_data);
    Pixel *** template_images = (Pixel ***) malloc((num_of_arguments - 1) * sizeof(Pixel **));
    for (int i = 0; i < num_of_arguments - 1; i++)
        template_images[i] = load_image(templates_bitmap[i], templates_bitmap_data[i]);
    
    // call function to calculate correlation between image and templates




    /* TESTING */
/*
    FILE * out = fopen("testing.bmp", "wb");
    int k = 0;
    unsigned char R,G,B;
    fwrite(main_bitmap_data->header, 54, 1, out);
    for (int i = main_bitmap_data->height - 1; i >= 0; i--)
        for (int j = 0; j < main_bitmap_data->width; j++)
        {
            fread(&R, 1, 1, main_bitmap);
            fread(&G, 1, 1, main_bitmap);
            fread(&B, 1, 1, main_bitmap);
            if (i > main_bitmap_data->height - 10 && j < 500)
            {
                R = G = B = 255;
                k++;
            }
            fwrite(&B, 1, 1, out);
            fwrite(&G, 1, 1, out);
            fwrite(&R, 1, 1, out);
            //fwrite(&main_image[i][j].R, 1, 1, out);
            //fwrite(&main_image[i][j].G, 1, 1, out);
            //fwrite(&main_image[i][j].B, 1, 1, out);
        }
    fclose(out);
*/
    /* END OF TESTING */

    fclose(main_bitmap);
    for (int i = 0; i < num_of_arguments - 1; i++) 
        fclose(templates_bitmap[i]);
}

Pixel ** load_image(FILE * tmp, BMP_info * bitmap_data)
{
    Pixel ** matrix_bitmap = (Pixel **) malloc(bitmap_data->height * sizeof(Pixel *));
    if (check_memory_allocation(matrix_bitmap)) return NULL;

    for (int i = 0; i < bitmap_data->height; i++)
    { 
       *(matrix_bitmap + i) = (Pixel *) malloc(bitmap_data->width * sizeof(Pixel));
        if (check_memory_allocation(*(matrix_bitmap + i))) return NULL;
    }

    for (int i = bitmap_data->height - 1; i >= 0; i--)
        for (int j = 0; j < bitmap_data->width; j++)
        {
            fread(&matrix_bitmap[i][j].B, 1, 1, tmp);
            fread(&matrix_bitmap[i][j].G, 1, 1, tmp);
            fread(&matrix_bitmap[i][j].R, 1, 1, tmp);
        }

    return matrix_bitmap;
}

BMP_info * get_bitmap_data(FILE * tmp)
{
    BMP_info * bitmap_data = (BMP_info *) malloc(sizeof(BMP_info));
    if (check_memory_allocation(bitmap_data)) return NULL;
    bitmap_data->header = (unsigned char *) malloc(54 * sizeof(unsigned char));
    fread(bitmap_data->header, 54, 1, tmp);
    fseek(tmp, 2, SEEK_SET);
    fread(&(bitmap_data->num_bytes), sizeof(unsigned int), 1, tmp);
    fseek(tmp, 18, SEEK_SET);
    fread(&(bitmap_data->width), sizeof(unsigned int), 1, tmp);
    fread(&(bitmap_data->height), sizeof(unsigned int), 1, tmp);
    return bitmap_data;
}

unsigned char check_file_error_null(FILE * tmp)
{
    if (!tmp)
    {
        printf("\nFile not found\n");
        return 1;
    }
    return 0;
}

unsigned char check_memory_allocation(const void * data)
{
    if (!data)
    {
        printf("Couldn't allocate memory");
        return 1;
    }
    return 0;
}
