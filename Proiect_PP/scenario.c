#include "template_matching.c"

void crypt_choice(char * path, char * destination, char * key);
void decrypt_choice(char * path, char * destination, char * key);
void init_memory(char ** cr1, char ** cr2, char ** dc1, char ** dc2, char ** k1, char ** k2);

int main()
{      
    char * to_crypt_name, * to_place_crypt_name, * to_decrypt_name, * to_place_decrypt_name; 
    char * secret_key_crypt, * secret_key_decrypt;
    init_memory(&to_crypt_name, &to_place_crypt_name, &to_decrypt_name, &to_place_decrypt_name, &secret_key_crypt, &secret_key_decrypt); 
    
    crypt_choice(to_crypt_name, to_place_crypt_name, secret_key_crypt);
    encrypt_file(to_crypt_name, to_place_crypt_name, secret_key_crypt);

    decrypt_choice(to_decrypt_name, to_place_decrypt_name, secret_key_decrypt);
    decrypt_file(to_decrypt_name, to_place_decrypt_name, secret_key_decrypt);

    recongnize_patterns(12, "images/digits.bmp", "images/found_patterns.bmp","images/digit_templates/cifra0.bmp",
     "images/digit_templates/cifra1.bmp", "images/digit_templates/cifra2.bmp", "images/digit_templates/cifra3.bmp", 
     "images/digit_templates/cifra4.bmp", "images/digit_templates/cifra5.bmp", "images/digit_templates/cifra6.bmp",
     "images/digit_templates/cifra7.bmp", "images/digit_templates/cifra8.bmp" , "images/digit_templates/cifra9.bmp");

     free(to_crypt_name);
     free(to_place_crypt_name);
     free(to_decrypt_name);
     free(to_place_decrypt_name);
     free(secret_key_crypt);
     free(secret_key_decrypt);
}

void crypt_choice(char * path, char * destination, char * key)
{
    printf("\nEnter the name of the file you want to encrypt: ");
    scanf("%30s", path);

    printf("Enter the name of the encrypted file: ");
    scanf("%30s", destination);

    printf("Enter the name of the file with the secret keys: ");
    scanf("%30s", key);
    printf("\n");
}

void decrypt_choice(char * path, char * destination, char * key)
{
    printf("\nEnter the name of the file you want to decrypt: ");
    scanf("%30s", path);

    printf("Enter the name of the decrypted file: ");
    scanf("%30s", destination);

    printf("Enter the name of the file with the secret keys: ");
    scanf("%30s", key);
    printf("\n");
}

void init_memory(char ** cr1, char ** cr2, char ** dc1, char ** dc2, char ** k1, char ** k2)
{
    (*cr1)= (char *) malloc(30 * sizeof(char));
    if (check_memory_allocation(*cr1)) return;

    (*cr2) = (char *) malloc(30 * sizeof(char));
    if (check_memory_allocation(cr2)) return;

    (*dc1) = (char *) malloc(30 * sizeof(char));
    if (check_memory_allocation(*dc1)) return;

    (*dc2) = (char *) malloc(30 * sizeof(char));
    if (check_memory_allocation(*dc2)) return;

    (*k1) = (char *) malloc(30 * sizeof(char));
    if (check_memory_allocation(*k1)) return;

    (*k2) = (char *) malloc(30 * sizeof(char));
    if (check_memory_allocation(*k2)) return;
}
