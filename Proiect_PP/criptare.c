#include <stdlib.h>
#include <stdio.h>

typedef struct Pixel {
    unsigned char R, G, B;
} Pixel;

typedef struct BMP_info {
    unsigned int num_bytes, width, height;
    unsigned char * header;
} BMP_info;

void load_out_BMP(const char * BMP_name);
unsigned char check_file_error_null(FILE * tmp);
unsigned char * load_in_BMP(const char * BMP_name);
float * chi_squared_result(Pixel * bitmap_array, unsigned long size);
void chi_squared(Pixel * original_bitmap, Pixel * encrypted_bitmap, unsigned long size);

void encrypt_file(const char * BMP_initial, const char * BMP_encrypt, const char * secret_key);
void decrypt_file(const char * BMP_encrypt, const char * BMP_decrypt, const char * secret_key);

BMP_info * get_bitmap_data(FILE * tmp);
void xorshift32(unsigned int * current_state);
Pixel pixel_xor_pixel(Pixel pixel1, Pixel pixel2);
Pixel pixel_xor_uint(Pixel pixel, unsigned int uint);
Pixel * liniar_bitmap(FILE * tmp, BMP_info * bitmap_data);
unsigned long * inverse_permutation(unsigned long * sequence, unsigned long size);
void display_result_image(FILE * out, Pixel * image_array, BMP_info * bitmap_data);
unsigned int * generate_random_sequence(unsigned long sequeance_size, FILE * secret_key);
void durstenfeld_shuffle(unsigned long * seq, unsigned int * random_sequence, unsigned long size);
Pixel * apply_permutation(Pixel * original_bitmap, unsigned long * permutation, unsigned long size);
Pixel * create_decyphered_image(Pixel * cyphered_bitmap, unsigned int * random_sequence, BMP_info * bitmap_data, FILE * secret_key);
void create_cyphered_image(Pixel * shuffled_bitmap, unsigned int * random_sequence, BMP_info * bitmap_data, FILE * out, FILE * secret_key);

int main()
{
    encrypt_file("peppers.bmp", "test.bmp", "secret_key.txt");
}

void chi_squared(Pixel * original_bitmap, Pixel * encrypted_bitmap, unsigned long size)
{
    float * original_chi_squared = chi_squared_result(original_bitmap, size);
    float * encrypted_chi_squared = chi_squared_result(encrypted_bitmap, size);
    printf("chi squared test for initial bitmap: (%.2f, %.2f, %.2f)\n", *original_chi_squared, *(original_chi_squared + 1), *(original_chi_squared + 2));
    printf("chi squared test for encrypted bitmap: (%.2f, %.2f, %.2f)\n", *encrypted_chi_squared, *(encrypted_chi_squared + 1), *(encrypted_chi_squared + 2));
}

float * chi_squared_result(Pixel * bitmap_array, unsigned long size)
{
    float theoretical_frequency = size / 256;
    float * chi_squared = (float *) calloc(3, sizeof(float));
    unsigned long * R = (unsigned long *) calloc(255, sizeof(unsigned long));
    unsigned long * G = (unsigned long *) calloc(255, sizeof(unsigned long));
    unsigned long * B = (unsigned long *) calloc(255, sizeof(unsigned long));
    for (int i = 0; i < size; i++)
    {
        R[bitmap_array[i].R]++;
        G[bitmap_array[i].G]++;
        B[bitmap_array[i].B]++;
    }
    for (int i = 0; i < 255; i++)
    {
        chi_squared[0] += (R[i] - theoretical_frequency) * (R[i] - theoretical_frequency) / theoretical_frequency;
        chi_squared[1] += (G[i] - theoretical_frequency) * (G[i] - theoretical_frequency) / theoretical_frequency;
        chi_squared[2] += (B[i] - theoretical_frequency) * (B[i] - theoretical_frequency) / theoretical_frequency;        
    }
    return chi_squared;
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

    // closing and freeing memory
    fclose(in); fclose(out); fclose(key);
    free(seq); free(bitmap_data); free(image_array); free(random_sequence); free(shuffled_bitmap);
}

void decrypt_file(const char * BMP_encrypt, const char * BMP_decrypt, const char * secret_key)
{
    FILE * in = fopen(BMP_encrypt, "rb");
    FILE * out = fopen(BMP_decrypt, "wb");
    FILE * key = fopen(secret_key, "r");

    if ((check_file_error_null(in) & 1) == 1 || (check_file_error_null(out) & 1) == 1 || (check_file_error_null(key) & 1) == 1) return;

    // 0) get data from encrypted file
    BMP_info * bitmap_data = get_bitmap_data(in);
    Pixel * image_array = liniar_bitmap(in, bitmap_data);

    // 1) generate random sequeance of 2*w*h-1 elements with xorshift32
    unsigned int * random_sequence = generate_random_sequence(2 * bitmap_data->width * bitmap_data->height, key);

    // 2) generate random permutation for the first w*h-1 elements from random sequence
    unsigned long * seq = (unsigned long *) malloc(bitmap_data->width * bitmap_data->height * sizeof(unsigned long)); 
    durstenfeld_shuffle(seq, random_sequence, bitmap_data->width * bitmap_data->height);
    seq = inverse_permutation(seq, bitmap_data->width * bitmap_data->height); // bad

    // 3) create decyphered image
    Pixel * decyphered_image = create_decyphered_image(image_array, random_sequence, bitmap_data, key);

    // 4) apply permutation to decyphered image and display result
    Pixel * decyphered_image_result = apply_permutation(decyphered_image, seq, bitmap_data->width * bitmap_data->height);
    display_result_image(out, decyphered_image_result, bitmap_data);
    
    // closing and freeing memory
    fclose(in); fclose(out); fclose(key);
    free(seq); free(bitmap_data); free(image_array); free(random_sequence); free(decyphered_image); free(decyphered_image_result);
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

    display_result_image(out, cyphered_image, bitmap_data);
}

Pixel * create_decyphered_image(Pixel * cyphered_bitmap, unsigned int * random_sequence, BMP_info * bitmap_data, FILE * secret_key)
{
    unsigned int starting_value;
    unsigned long random_sequence_start = bitmap_data->width * bitmap_data->height;
    fscanf(secret_key, "%u", &starting_value);

    Pixel * decyphered_image = (Pixel *) malloc(bitmap_data->width * bitmap_data->height * sizeof(Pixel));
    Pixel temp = pixel_xor_uint(*cyphered_bitmap, starting_value);
    *decyphered_image = pixel_xor_uint(temp, *(random_sequence + random_sequence_start));

    for (int i = 1; i < random_sequence_start; i++)
    {
        temp = pixel_xor_pixel(*(cyphered_bitmap + i - 1), *(cyphered_bitmap + i));
        *(decyphered_image + i) = pixel_xor_uint(temp, *(random_sequence + random_sequence_start + i));
    }
    return decyphered_image;
}

void display_result_image(FILE * out, Pixel * image_array, BMP_info * bitmap_data)
{
    fwrite(bitmap_data->header, 54, 1, out);
    for (int i = bitmap_data->height - 1; i >= 0; i--)
        for (int j = 0; j < bitmap_data->width; j++)
        {
            fwrite(&image_array[i * bitmap_data->width + j].B, 1, 1, out);
            fwrite(&image_array[i * bitmap_data->width + j].G, 1, 1, out);
            fwrite(&image_array[i * bitmap_data->width + j].R, 1, 1, out);
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

unsigned long * inverse_permutation(unsigned long * sequeance, unsigned long size) 
{
    unsigned long * result = (unsigned long *) malloc(size * sizeof(unsigned long));
    for (int i = 0; i < size; i++) 
        for (int j = 0; j < size; j++) 
            if (*(sequeance + j) == i) 
            {  
                result[i] = j; 
                break; 
            } 
    return result;
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