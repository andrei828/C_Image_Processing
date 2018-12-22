#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#define RED 0
#define YELLOW 1
#define GREEN 2
#define CYAN 3
#define MAGENTA1 4
#define BLUE1 5
#define SILVER 6
#define BLUE2 7
#define MAGENTA2 8
#define BLUE3 9

typedef struct Window {
    unsigned int X_coord, Y_coord;
} Window;

typedef struct Window_List {
    Window * window;
    struct Window_List * next_window;
} Window_List;

typedef struct Pixel {
    unsigned char R, G, B;
} Pixel;

typedef struct BMP_info {
    unsigned int num_bytes, width, height;
    unsigned char * header;
} BMP_info;

Pixel * init_color_palette();
BMP_info * get_bitmap_data(FILE * tmp);
void recongnize_patterns(int num_of_arguments, ...);
Pixel ** load_image(FILE * tmp, BMP_info * bitmap_data);
void border_window(Pixel ** main_image, Window * window_center, BMP_info * template_data, Pixel * color);

double template_average_grayscale_intensity(Pixel ** template_image, BMP_info * template_data);
double main_average_grayscale_intensity(Pixel ** main_image, Window * window_center, BMP_info * template_data);

double template_deviation_of_pixel_intensity(Pixel ** template_image, BMP_info * template_data, double average_intensity);
double main_deviation_of_pixel_intensity(Pixel ** main_image, Window * window_center, BMP_info * template_data, double average_intensity);

Window_List * calculate_correlation(Pixel ** main_image, Pixel ** template_image, BMP_info * main_image_data, BMP_info * template_image_data, double precision);

unsigned char check_file_error_null(FILE * tmp);
unsigned char check_memory_allocation(const void * data);


//    functions for linked list of windows implementation
void push(Window_List ** start_window, Window * add_window);


int main()
{
    recongnize_patterns(11, "test_grayscale.bmp", "cifra0.bmp", "cifra1.bmp",
     "cifra2.bmp", "cifra3.bmp", "cifra4.bmp", "cifra5.bmp", "cifra6.bmp",
     "cifra7.bmp", "cifra8.bmp" , "cifra9.bmp");
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
        if (check_file_error_null(templates_bitmap[i])) return;
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
    Pixel * color_palette = init_color_palette();
    for (int digit = 0; digit < num_of_arguments - 1; digit++)
    {
        Window_List * window_list = calculate_correlation(main_image, template_images[digit], main_bitmap_data, templates_bitmap_data[digit], 0.50);
        while (window_list)
        {
            Window_List * to_delete = window_list;
            border_window(main_image, window_list->window, templates_bitmap_data[digit], &color_palette[digit]);
            window_list = window_list->next_window;
            free(to_delete);
        }
    }

    /* TESTING */
    int padding;
    if (main_bitmap_data->width % 4 != 0) padding = 4 - (3 * main_bitmap_data->width) % 4;
    else padding = 0;
    unsigned char BLANK = 0;

    FILE * out = fopen("testing.bmp", "wb");
    fwrite(main_bitmap_data->header, 54, 1, out);
    for (int i = main_bitmap_data->height - 1; i >= 0; i--)
    {
        for (int j = 0; j < main_bitmap_data->width; j++)
        {
            fwrite(&main_image[i][j].B, 1, 1, out);
            fwrite(&main_image[i][j].G, 1, 1, out);
            fwrite(&main_image[i][j].R, 1, 1, out);
        }
        fwrite(&BLANK, 1, padding, out);
    }
    fclose(out);

    /* END OF TESTING */

    fclose(main_bitmap);
    for (int i = 0; i < num_of_arguments - 1; i++) 
        fclose(templates_bitmap[i]);
}

void border_window(Pixel ** main_image, Window * window_center, BMP_info * template_data, Pixel * color)
{
    unsigned int start_i = window_center->Y_coord - (template_data->height / 2);
    unsigned int start_j = window_center->X_coord - (template_data->width  / 2);

    unsigned int end_i = window_center->Y_coord + (template_data->height / 2);
    unsigned int end_j = window_center->X_coord + (template_data->width  / 2);
    
    if (template_data->height % 2) end_i++;
    if (template_data->width  % 2) end_j++;

    for (int i = start_i; i < end_i; i++)
    {
        main_image[i][start_j] = *color;
        main_image[i][end_j]   = *color;
    }

    for (int j = start_j; j <= end_j; j++)
    {
        main_image[start_i][j] = *color;
        main_image[end_i][j]   = *color;
    }
}

Window_List * calculate_correlation(Pixel ** main_image, Pixel ** template_image, BMP_info * main_image_data, BMP_info * template_image_data, double precision)
{
    Window_List * valid_windows_list = NULL;

    unsigned int template_num_of_pixels = template_image_data->width * template_image_data->height;
    unsigned int start_i = template_image_data->height / 2; 
    unsigned int start_j = template_image_data->width  / 2; 
    unsigned int end_i   = main_image_data->height - (template_image_data->height / 2) - 1;
    unsigned int end_j   = main_image_data->width  - (template_image_data->width  / 2) - 1;

    double correlation, tmp;
    double template_average_intensity = template_average_grayscale_intensity(template_image, template_image_data);
    double sigma_s = template_deviation_of_pixel_intensity(template_image, template_image_data, template_average_intensity);
    // iterate template over image
    for (unsigned int i_main = start_i; i_main < end_i; i_main++)
        for (unsigned int j_main = start_j; j_main < end_j; j_main++)
        {
            Window * new_window = (Window *) malloc(sizeof(Window));
            new_window->X_coord = j_main;
            new_window->Y_coord = i_main;            
            correlation = 0;

            double window_average_intensity = main_average_grayscale_intensity(main_image, new_window, template_image_data);
            double sigma_f1 = main_deviation_of_pixel_intensity(main_image, new_window, template_image_data, window_average_intensity);
            
            unsigned int window_start_i = new_window->Y_coord - (template_image_data->height / 2);
            unsigned int window_start_j = new_window->X_coord - (template_image_data->width  / 2);

            unsigned int window_end_i = new_window->Y_coord + (template_image_data->height / 2);
            unsigned int window_end_j = new_window->X_coord + (template_image_data->width  / 2);
    
            if (template_image_data->height % 2) window_end_i++;
            if (template_image_data->width  % 2) window_end_j++;

            for (unsigned int i_window = window_start_i, i_template = 0; i_window < window_end_i && i_template < template_image_data->height; i_window++, i_template++)
                for (unsigned int j_window = window_start_j, j_template = 0; j_window < window_end_j && j_template < template_image_data->width; j_window++, j_template++)
                {
                    tmp = ((main_image[i_window][j_window].R - window_average_intensity) * (template_image[i_template][j_template].R - template_average_intensity)) / (sigma_s * sigma_f1);
                    correlation += tmp;
                }
            correlation = correlation / template_num_of_pixels;

            if (correlation > precision)  push(&valid_windows_list, new_window);
            else free(new_window);
        }

    return valid_windows_list;
}

/*
starting point at
start_i = [Y_coord - (template_height  / 2)]
start_j = [X_coord - (template_width / 2)]

ending point at 
end_i = [Y_coord + (template_height  / 2)]
end_j = [X_coord + (template_width / 2)]

if template_height % 2: 
    end_i++;
if template_width % 2:
    end_j++;

for (int i = start_i; i < end_i; i++)
    for (int j = start_j; j < end_j; j++)
        ...
*/

double template_average_grayscale_intensity(Pixel ** template_image, BMP_info * template_data)
{
    unsigned long pixel_value_sum = 0;
    
    for (int i = 0; i < template_data->height; i++)
        for (int j = 0; j < template_data->width; j++)
            pixel_value_sum += template_image[i][j].R;
    return pixel_value_sum / template_data->width / template_data->height;
}

double main_average_grayscale_intensity(Pixel ** main_image, Window * window_center, BMP_info * template_data)
{
    unsigned long pixel_value_sum = 0;

    unsigned int start_i = window_center->Y_coord - (template_data->height / 2);
    unsigned int start_j = window_center->X_coord - (template_data->width  / 2);

    unsigned int end_i = window_center->Y_coord + (template_data->height / 2);
    unsigned int end_j = window_center->X_coord + (template_data->width  / 2);
    
    if (template_data->height % 2) end_i++;
    if (template_data->width  % 2) end_j++;
    
    for (unsigned int i = start_i; i < end_i; i++)
        for (unsigned int j = start_j; j < end_j; j++)
            pixel_value_sum += main_image[i][j].R;
    
    return pixel_value_sum / template_data->width / template_data->height;
}

double template_deviation_of_pixel_intensity(Pixel ** template_image, BMP_info * template_data, double average_intensity)
{
    double sigma_s = 0;
    for (int i = 0; i < template_data->height; i++)
        for (int j = 0; j < template_data->width; j++)
            sigma_s = sigma_s + (template_image[i][j].R - average_intensity) * (template_image[i][j].R - average_intensity);

    sigma_s = sigma_s / (template_data->width * template_data->height - 1);
    return sqrt(sigma_s);
}

double main_deviation_of_pixel_intensity(Pixel ** main_image, Window * window_center, BMP_info * template_data, double average_intensity)
{
    double sigma_f1 = 0;
    
    unsigned int start_i = window_center->Y_coord - (template_data->height / 2);
    unsigned int start_j = window_center->X_coord - (template_data->width  / 2);

    unsigned int end_i = window_center->Y_coord + (template_data->height / 2);
    unsigned int end_j = window_center->X_coord + (template_data->width  / 2);
    
    if (template_data->height % 2) end_i++;
    if (template_data->width  % 2) end_j++;
    
    for (unsigned int i = start_i; i < end_i; i++)
        for (unsigned int j = start_j; j < end_j; j++)
            sigma_f1 = sigma_f1 + (main_image[i][j].R - average_intensity) * (main_image[i][j].R - average_intensity);
       
    sigma_f1 = sigma_f1 / (template_data->width * template_data->height - 1);
    return sqrt(sigma_f1);
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

    int padding;
    if (bitmap_data->width % 4 != 0) padding = 4 - (3 * bitmap_data->width) % 4;
    else padding = 0;

    fseek(tmp, 54, SEEK_SET);
    for (int i = bitmap_data->height - 1; i >= 0; i--)
    {
        for (int j = 0; j < bitmap_data->width; j++)
        {
            fread(&matrix_bitmap[i][j].R, 1, 1, tmp);
            fread(&matrix_bitmap[i][j].G, 1, 1, tmp);
            fread(&matrix_bitmap[i][j].B, 1, 1, tmp);
        }
        fseek(tmp, padding, SEEK_CUR);
    }
    return matrix_bitmap;
}

BMP_info * get_bitmap_data(FILE * tmp)
{
    BMP_info * bitmap_data = (BMP_info *) malloc(sizeof(BMP_info));
    if (check_memory_allocation(bitmap_data)) return NULL;
    fseek(tmp, 0, SEEK_SET);
    bitmap_data->header = (unsigned char *) malloc(54 * sizeof(unsigned char));
    fread(bitmap_data->header, 54, 1, tmp);
    fseek(tmp, 2, SEEK_SET);
    fread(&(bitmap_data->num_bytes), sizeof(unsigned int), 1, tmp);
    fseek(tmp, 18, SEEK_SET);
    fread(&(bitmap_data->width), sizeof(unsigned int), 1, tmp);
    fread(&(bitmap_data->height), sizeof(unsigned int), 1, tmp);
    return bitmap_data;
}

Pixel * init_color_palette()
{
    Pixel * color_palette = (Pixel *) malloc(sizeof(Pixel));
    if (check_memory_allocation(color_palette)) return NULL;
    
    color_palette[RED].R      = 255; color_palette[RED].G      =   0; color_palette[RED].B      =   0;
    color_palette[CYAN].R     =   0; color_palette[CYAN].G     = 255; color_palette[CYAN].B     = 255;
    color_palette[BLUE1].R    =   0; color_palette[BLUE1].G    =   0; color_palette[BLUE1].B    = 255;
    color_palette[BLUE2].R    = 255; color_palette[BLUE2].G    = 140; color_palette[BLUE2].B    =   0;
    color_palette[BLUE3].R    = 128; color_palette[BLUE3].G    =   0; color_palette[BLUE3].B    =   0;
    color_palette[GREEN].R    =   0; color_palette[GREEN].G    = 255; color_palette[GREEN].B    =   0;
    color_palette[SILVER].R   = 192; color_palette[SILVER].G   = 192; color_palette[SILVER].B   = 192;
    color_palette[YELLOW].R   = 255; color_palette[YELLOW].G   = 255; color_palette[YELLOW].B   =   0;
    color_palette[MAGENTA1].R = 255; color_palette[MAGENTA1].G =   0; color_palette[MAGENTA1].B = 255;
    color_palette[MAGENTA2].R = 128; color_palette[MAGENTA2].G =   0; color_palette[MAGENTA2].B = 128;

    return color_palette;
}

void push(Window_List ** start_window, Window * add_window)
{
    if (*start_window == NULL)
    {
        *start_window = (Window_List *) malloc(sizeof(Window_List));
        (*start_window)->window = add_window;
        (*start_window)->next_window = NULL;
    }
    
    Window_List * iteration_copy = *start_window;
    while (iteration_copy->next_window)
        iteration_copy = iteration_copy->next_window;
    iteration_copy->next_window = (Window_List *) malloc(sizeof(Window_List));
    iteration_copy->next_window->window = add_window;
    iteration_copy->next_window->next_window = NULL;
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