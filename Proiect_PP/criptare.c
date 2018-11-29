#include <stdlib.h>
#include <stdio.h>

typedef struct Pixel {
    int R, G, B;
} Pixel;

unsigned char * load_in_BMP(const char * BMP_name);
void load_out_BMP(const char * BMP_name);
void encrypt_file(const char * BMP_initial, const char * BMP_encrypt, const char * secret_key);
unsigned int * generate_random_sequence(unsigned long image_size, FILE * secret_key);
void durstenfeld_shuffle(unsigned int * random_sequence, unsigned long size);
unsigned int xorshift32(unsigned int current_state);
unsigned int * file_num_bytes(FILE * tmp);
unsigned char check_file_error_null(FILE * tmp);

int main()
{
    encrypt_file("peppers.bmp", "peppers.bmp", "secret_key.txt");
}

void encrypt_file(const char * BMP_initial, const char * BMP_encrypt, const char * secret_key)
{
    FILE * in = fopen(BMP_initial, "rb");
    FILE * out = fopen(BMP_encrypt, "wb");
    FILE * key = fopen(secret_key, "r");
    
    if ((check_file_error_null(in) & 1) == 1 || (check_file_error_null(out) & 1) == 1 || (check_file_error_null(key) & 1) == 1)
        return;

    // get num of bytes from file with header ***
    unsigned int * BMP_num_bytes = file_num_bytes(in);

    // generate random sequeance of 2*w*h-1 elements with xorshift32
    unsigned int * random_sequence = generate_random_sequence(BMP_num_bytes, key);

    // generate random permutation for the first w*h-1 elements from random sequence

    fclose(in);
    fclose(out);
    fclose(key);
}

unsigned int * generate_random_sequence(unsigned long image_size, FILE * secret_key)
{
    unsigned long sequence_length = 2 * image_size;
    unsigned int temp_random, * sequence = (unsigned int *) malloc(sequence_length * sizeof(unsigned int) + 1);
    fscanf(secret_key, "%d", sequence);

    for (unsigned long i = 0; i < sequence_length; i++)
    {
        temp_random = *(sequence + i);
        *(sequence + i + 1) = xorshift32(temp_random);
    }
    return sequence;
}

unsigned int xorshift32(unsigned int current_state)
{
    current_state ^= current_state << 13;
    current_state ^= current_state >> 17;
    current_state ^= current_state << 5;
    return current_state;
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

unsigned int * file_num_bytes(FILE * tmp)
{
    unsigned int * num_bytes = (unsigned int *) malloc(sizeof(unsigned int));
    fseek(tmp, 2, SEEK_SET);
    fread(&num_bytes, sizeof(unsigned int), 1, tmp);
    return num_bytes;
}

unsigned char * load_in_BMP(const char * BMP_name)
{
    FILE * BMP_file = fopen(BMP_name, "rb");
    if ((check_file_error_null(BMP_file) & 1) == 1)
        return NULL;

    fseek(BMP_file, 0, SEEK_END);
    unsigned long BMP_length = ftell(BMP_file);
    unsigned char * BMP_liniar = (unsigned char *) malloc(BMP_length * sizeof(char));
    if (!BMP_liniar)
    {
        printf("\nCouldn't allocate memory\n");
        return NULL;
    }

    fseek(BMP_file, 0, SEEK_SET);
    fread(BMP_liniar, BMP_length, 1, BMP_file);
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