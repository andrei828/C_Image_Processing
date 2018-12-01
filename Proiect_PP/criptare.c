#include <stdlib.h>
#include <stdio.h>

typedef struct Pixel {
    unsigned char R, G, B;
} Pixel;

typedef struct BMP_info {
    unsigned int num_bytes, width, height;
    unsigned char * header;
} BMP_info;

unsigned char * load_in_BMP(const char * BMP_name);
void load_out_BMP(const char * BMP_name);

void encrypt_file(const char * BMP_initial, const char * BMP_encrypt, const char * secret_key);
unsigned int * generate_random_sequence(unsigned long sequeance_size, FILE * secret_key);
void durstenfeld_shuffle(unsigned int * random_sequence, unsigned long size);
void xorshift32(unsigned int * current_state);
void create_cyphered_image(Pixel * shuffled_bitmap, unsigned int * random_sequence, BMP_info * bitmap_data, FILE * out, FILE * secret_key);
Pixel * pixel_xor_uint(Pixel * pixel, unsigned int uint);
Pixel * liniar_bitmap(FILE * tmp, unsigned long size);
Pixel * apply_permutation(Pixel * original_bitmap, unsigned long * permutation, unsigned long size);
BMP_info * get_bitmap_data(FILE * tmp);
unsigned char check_file_error_null(FILE * tmp);

int main()
{
    encrypt_file("peppers.bmp", "test.bmp", "secret_key.txt");
}

void encrypt_file(const char * BMP_initial, const char * BMP_encrypt, const char * secret_key)
{
    FILE * in = fopen(BMP_initial, "rb");
    FILE * out = fopen(BMP_encrypt, "wb");
    FILE * key = fopen(secret_key, "r");
    
    if ((check_file_error_null(in) & 1) == 1 || (check_file_error_null(out) & 1) == 1 || (check_file_error_null(key) & 1) == 1)
        return; 

    // get data from initial bitmap
    BMP_info * bitmap_data = get_bitmap_data(in);
    Pixel * image_array = liniar_bitmap(in, bitmap_data->width * bitmap_data->height);
    
    // generate random sequeance of 2*w*h-1 elements with xorshift32
    unsigned int * random_sequence = generate_random_sequence(2 * bitmap_data->height * bitmap_data->width , key);
    
    // generate random permutation for the first w*h-1 elements from random sequence
    

    // apply permutation to liniar bitmap (call function apply_permutation)

    fclose(in);
    fclose(out);
    fclose(key);
    free(bitmap_data);
    free(image_array);
    free(random_sequence);
}

void create_cyphered_image(Pixel * shuffled_bitmap, unsigned int * random_sequence, BMP_info * bitmap_data, FILE * out, FILE * secret_key)
{

}

Pixel * pixel_xor_uint(Pixel * pixel, unsigned int uint)
{
    Pixel * new_Pixel = (Pixel *) malloc(sizeof(Pixel));
    new_Pixel->B = pixel->B ^ (uint & 15);
    uint >>= 4;
    new_Pixel->G = pixel->G ^ (uint % 15);
    uint >>= 4;
    new_Pixel->R = pixel->R ^ (uint % 15);
    return new_Pixel;
}

Pixel * apply_permutation(Pixel * original_bitmap, unsigned long * permutation, unsigned long size)
{
    Pixel * new_image = (Pixel *) malloc(size * sizeof(Pixel));
    for (unsigned long i = 0; i < size; i++)
        *(new_image + *(permutation + i)) = *(original_bitmap + i);
    return new_image;
}

void durstenfeld_shuffle(unsigned int * random_sequence, unsigned long size)
{

}

unsigned int * generate_random_sequence(unsigned long sequence_size, FILE * secret_key)
{
    unsigned int temp_random, * sequence = (unsigned int *) malloc(sequence_size * sizeof(unsigned int));
    fscanf(secret_key, "%d", sequence);

    for (unsigned long i = 0; i < sequence_size-1; i++)
    {
        *(sequence + i + 1) = *(sequence + i);
        xorshift32(sequence + i + 1);
    }
    return sequence;
}

void xorshift32(unsigned int * current_state)
{
    *current_state ^= *current_state << 13;
    *current_state ^= *current_state >> 17;
    *current_state ^= *current_state << 5;
}

BMP_info * get_bitmap_data(FILE * tmp)
{
    BMP_info * bitmap_data = (BMP_info *) malloc(sizeof(BMP_info));
    bitmap_data -> header = (unsigned char *) malloc(54 * sizeof(unsigned char));
    fread(bitmap_data -> header, 54, 1, tmp);
    fseek(tmp, 2, SEEK_SET);
    fread(&(bitmap_data -> num_bytes), sizeof(unsigned int), 1, tmp);
    fseek(tmp, 18, SEEK_SET);
    fread(&(bitmap_data -> width), sizeof(unsigned int), 1, tmp);
    fread(&(bitmap_data -> height), sizeof(unsigned int), 1, tmp);
    return bitmap_data;
}

Pixel * liniar_bitmap(FILE * tmp, unsigned long size)
{
    Pixel * pixels = (Pixel *) malloc(size * sizeof(Pixel));
    fseek(tmp, 54, SEEK_SET);
    for (unsigned long i = 0; i < size; i++)
    {
        fread(&pixels[i].R, 1, 1, tmp);
        fread(&pixels[i].G, 1, 1, tmp);
        fread(&pixels[i].B, 1, 1, tmp);
    }
    return pixels;
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

unsigned char * load_in_BMP(const char * BMP_name)
{
    FILE * BMP_file = fopen(BMP_name, "rb");
    if ((check_file_error_null(BMP_file) & 1) == 1)
        return NULL;

    BMP_info * bitmap_data = get_bitmap_data(BMP_file);
    fseek(BMP_file, 54, SEEK_SET);

    unsigned char * BMP_liniar = (unsigned char *) malloc(bitmap_data->width * bitmap_data->height);
    if (!BMP_liniar)
    {
        printf("\nCouldn't allocate memory for linearization\n");
        return NULL;
    }
    fread(BMP_liniar, bitmap_data -> num_bytes - 54, 1, BMP_file);
    fclose(BMP_file);
    return BMP_liniar;
}

void load_out_BMP(const char * BMP_name)
{
    FILE * BMP_file = fopen(BMP_name, "wb");
    if (!BMP_file)
    {
        printf("\nCouldn't find BMP image\n");
        return;
    }
}