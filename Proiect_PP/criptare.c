#include <stdlib.h>
#include <stdio.h>

typedef struct Pixel {
    unsigned char R, G, B;
} Pixel;

typedef struct BMP_info {
    unsigned int num_bytes, width, height;
    unsigned char * header;
} BMP_info;

void encrypt_file(const char * BMP_initial, const char * BMP_encrypt, const char * secret_key);
unsigned int * generate_random_sequence(unsigned long sequeance_size, FILE * secret_key);
void durstenfeld_shuffle(unsigned long * seq, unsigned int * random_sequence, unsigned long size);
void xorshift32(unsigned int * current_state);
void create_cyphered_image(Pixel * shuffled_bitmap, unsigned int * random_sequence, BMP_info * bitmap_data, FILE * out, FILE * secret_key);
Pixel pixel_xor_uint(Pixel pixel, unsigned int uint);
Pixel pixel_xor_pixel(Pixel pixel1, Pixel pixel2);
Pixel * liniar_bitmap(FILE * tmp, BMP_info * bitmap_data);
Pixel * apply_permutation(Pixel * original_bitmap, unsigned long * permutation, unsigned long size);
BMP_info * get_bitmap_data(FILE * tmp);

unsigned char * load_in_BMP(const char * BMP_name);
void load_out_BMP(const char * BMP_name);
unsigned char check_file_error_null(FILE * tmp);

int main()
{
    encrypt_file("peppers.bmp", "enc_peppers.bmp", "secret_key.txt");
}

void encrypt_file(const char * BMP_initial, const char * BMP_encrypt, const char * secret_key)
{
    FILE * in = fopen(BMP_initial, "rb");
    FILE * out = fopen(BMP_encrypt, "wb");
    FILE * key = fopen(secret_key, "r");
    
    if ((check_file_error_null(in) & 1) == 1 || (check_file_error_null(out) & 1) == 1 || (check_file_error_null(key) & 1) == 1) return; 

    // 0) get data from initial bitmap
    BMP_info * bitmap_data = get_bitmap_data(in);
    Pixel * image_array = liniar_bitmap(in, bitmap_data);

    // 1) generate random sequeance of 2*w*h-1 elements with xorshift32
    unsigned int * random_sequence = generate_random_sequence(2 * bitmap_data->width * bitmap_data->height, key);
    
    // 2) generate random permutation for the first w*h-1 elements from random sequence
    unsigned long * seq = (unsigned long *) malloc(bitmap_data->width * bitmap_data->height * sizeof(unsigned long)); 
    durstenfeld_shuffle(seq, random_sequence, bitmap_data->width * bitmap_data->height);

    // 3) apply permutation to liniar bitmap
    Pixel * shuffled_bitmap = apply_permutation(image_array, seq, bitmap_data->width * bitmap_data->height);
    
    // 4) create cyphered image
    create_cyphered_image(shuffled_bitmap, random_sequence, bitmap_data, out, key);

    fclose(in);
    fclose(out);
    fclose(key);
    free(seq);
    free(bitmap_data);
    free(image_array);
    free(random_sequence);
    free(shuffled_bitmap);
}

void create_cyphered_image(Pixel * shuffled_bitmap, unsigned int * random_sequence, BMP_info * bitmap_data, FILE * out, FILE * secret_key)
{
    unsigned int starting_value;
    unsigned long random_sequence_start = bitmap_data->width * bitmap_data->height;
    fscanf(secret_key, "%u", &starting_value);

    Pixel * cyphered_image = (Pixel *) malloc(bitmap_data->width * bitmap_data->height * sizeof(Pixel));
    Pixel temp = pixel_xor_uint(*shuffled_bitmap, starting_value);
    *cyphered_image = pixel_xor_uint(temp, *(random_sequence + random_sequence_start));

    for (int i = 1; i < random_sequence_start; i++)
    {
        temp = pixel_xor_pixel(*(cyphered_image + i - 1), *(shuffled_bitmap + i));
        *(cyphered_image + i) = pixel_xor_uint(temp, *(random_sequence + random_sequence_start + i));
    }

    fwrite(bitmap_data->header, 54, 1, out);
    for (int i = bitmap_data->height - 1; i >= 0; i--)
        for (int j = 0; j < bitmap_data->width; j++)
        {
            fwrite(&cyphered_image[i * bitmap_data->width + j].B, 1, 1, out);
            fwrite(&cyphered_image[i * bitmap_data->width + j].G, 1, 1, out);
            fwrite(&cyphered_image[i * bitmap_data->width + j].R, 1, 1, out);
        }
}

Pixel pixel_xor_uint(Pixel pixel, unsigned int uint)
{
    Pixel new_Pixel;
    new_Pixel.B = pixel.B ^ (uint & 255);
    uint >>= 8;
    new_Pixel.G = pixel.G ^ (uint & 255);
    uint >>= 8;
    new_Pixel.R = pixel.R ^ (uint & 255);
    return new_Pixel;
}

Pixel pixel_xor_pixel(Pixel pixel1, Pixel pixel2)
{
    Pixel new_Pixel;
    new_Pixel.R = pixel1.R ^ pixel2.R;
    new_Pixel.G = pixel1.G ^ pixel2.G;
    new_Pixel.B = pixel1.B ^ pixel2.B;
    return new_Pixel;
}

Pixel * apply_permutation(Pixel * original_bitmap, unsigned long * permutation, unsigned long size)
{
    Pixel * new_image = (Pixel *) malloc(size * sizeof(Pixel));
    for (unsigned long i = 0; i < size; i++)
        *(new_image + *(permutation + i)) = *(original_bitmap + i);
    return new_image;
}

void durstenfeld_shuffle(unsigned long * seq, unsigned int * random_sequence, unsigned long size)
{
    for (int i = 0; i < size; i++) seq[i] = i;

    for (int i = size - 1, j, tmp; i > 0; i--)
    {
        j = *(random_sequence + size - i) % (i + 1);
        tmp = seq[i];
        seq[i] = seq[j];
        seq[j] = tmp;
    }
}

unsigned int * generate_random_sequence(unsigned long sequence_size, FILE * secret_key)
{
    unsigned int * sequence = (unsigned int *) malloc(sequence_size * sizeof(unsigned int));
    fscanf(secret_key, "%u", sequence);
    for (unsigned long i = 0; i < sequence_size - 1; i++)
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
    bitmap_data->header = (unsigned char *) malloc(54 * sizeof(unsigned char));
    fread(bitmap_data->header, 54, 1, tmp);
    fseek(tmp, 2, SEEK_SET);
    fread(&(bitmap_data->num_bytes), sizeof(unsigned int), 1, tmp);
    fseek(tmp, 18, SEEK_SET);
    fread(&(bitmap_data->width), sizeof(unsigned int), 1, tmp);
    fread(&(bitmap_data->height), sizeof(unsigned int), 1, tmp);
    return bitmap_data;
}

Pixel * liniar_bitmap(FILE * tmp, BMP_info * bitmap_data)
{
    unsigned long size = bitmap_data->width * bitmap_data->height;
    Pixel * pixels = (Pixel *) malloc(size * sizeof(Pixel));
    fseek(tmp, 54, SEEK_SET);

    for (int i = bitmap_data->height - 1; i >= 0; i--)
        for (int j = 0; j < bitmap_data->width; j++)
        {
            fread(&pixels[i * bitmap_data->width + j].B, 1, 1, tmp);
            fread(&pixels[i * bitmap_data->width + j].G, 1, 1, tmp);
            fread(&pixels[i * bitmap_data->width + j].R, 1, 1, tmp);
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

/*
void inv( int v[], int n)
{
    int now, next, prev;
    for(int i = 1; i <= n; i++)
    {
        if (v[i] < 0) { continue; }
        now = v[i];
        prev = i;
        while (v[now] > 0)
        {
            next = v[now];
            v[now] = -prev;
            prev = now;
            now = next;
        }
    }
    for (int i = 1; i <= n; ++i)
        v[i] *= -1;
}
*/
