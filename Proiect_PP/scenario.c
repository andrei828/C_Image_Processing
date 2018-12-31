#include "template_matching.c"
#define READ_FILE_NAME "file_names.txt"

void init_memory_patterns(char ** m1, char ** f1, char *** t1);
void crypt_choice(char * path, char * destination, char * key, FILE * in);
void decrypt_choice(char * path, char * destination, char * key, FILE * in);
void patterns_choice(int * num_of_templates, char * main, char * found, char ** templates, FILE * in);
void init_memory_criptography(char ** cr1, char ** cr2, char ** dc1, char ** dc2, char ** k1, char ** k2);

int main()
{     
    char * to_crypt_name, * to_place_crypt_name, * to_decrypt_name, * to_place_decrypt_name; 
    char * secret_key_crypt, * secret_key_decrypt;
    char * main_patterns, * found_patterns, ** template_patterns;
    int  num_of_templates;

    FILE * in = fopen(READ_FILE_NAME, "r");
    if (check_file_error_null(in)) return -1;
    init_memory_patterns(&main_patterns, &found_patterns, &template_patterns);
    init_memory_criptography(&to_crypt_name, &to_place_crypt_name, &to_decrypt_name, &to_place_decrypt_name, &secret_key_crypt, &secret_key_decrypt);

    crypt_choice(to_crypt_name, to_place_crypt_name, secret_key_crypt, in);
    encrypt_file(to_crypt_name, to_place_crypt_name, secret_key_crypt);

    decrypt_choice(to_decrypt_name, to_place_decrypt_name, secret_key_decrypt, in);
    decrypt_file(to_decrypt_name, to_place_decrypt_name, secret_key_decrypt);

    patterns_choice(&num_of_templates, main_patterns, found_patterns, template_patterns, in);
    recongnize_patterns(num_of_templates, main_patterns, found_patterns, template_patterns);
    
    free(to_crypt_name);
    free(to_place_crypt_name);
    free(to_decrypt_name);
    free(to_place_decrypt_name);
    free(secret_key_crypt);
    free(secret_key_decrypt);
}

void crypt_choice(char * path, char * destination, char * key, FILE * in)
{
    fscanf(in, "Criptare:\n%40s", path);
    fscanf(in, "%40s", destination);
    fscanf(in, "%40s\n", key);
}

void decrypt_choice(char * path, char * destination, char * key, FILE * in)
{
    fscanf(in, "Decriptare:\n%40s", path);
    fscanf(in, "%40s", destination);
    fscanf(in, "%40s\n", key);
}

void patterns_choice(int * num_of_templates, char * main, char * found, char ** templates, FILE * in)
{
    fscanf(in, "Recunoastere patternuri:\n%40s", main);
    fscanf(in, "%40s", found);
    fscanf(in, "%d", num_of_templates);
    for (int i = 0; i < *num_of_templates; i++)
        fscanf(in, "%40s", templates[i]);
}

void init_memory_patterns(char ** m1, char ** f1, char *** t1)
{
    (*m1) = (char *) malloc(40 * sizeof(char));
    if (check_memory_allocation(*m1)) return;

    (*f1) = (char *) malloc(40 * sizeof(char));
    if (check_memory_allocation(*f1)) return;

    *t1 = (char **) malloc(10 * sizeof(char *)); 
    for (int i = 0; i < 10; i++)
        (*t1)[i] = (char *) malloc(40 * sizeof(char));
}

void init_memory_criptography(char ** cr1, char ** cr2, char ** dc1, char ** dc2, char ** k1, char ** k2)
{
    (*cr1)= (char *) malloc(40 * sizeof(char));
    if (check_memory_allocation(*cr1)) return;

    (*cr2) = (char *) malloc(40 * sizeof(char));
    if (check_memory_allocation(cr2)) return;

    (*dc1) = (char *) malloc(40 * sizeof(char));
    if (check_memory_allocation(*dc1)) return;

    (*dc2) = (char *) malloc(40 * sizeof(char));
    if (check_memory_allocation(*dc2)) return;

    (*k1) = (char *) malloc(40 * sizeof(char));
    if (check_memory_allocation(*k1)) return;

    (*k2) = (char *) malloc(40 * sizeof(char));
    if (check_memory_allocation(*k2)) return;
}
