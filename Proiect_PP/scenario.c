#include "template_matching.c"

int main()
{       
    // encrypting/decrypting and displaying chi squared test
    encrypt_file("images/digits.bmp", "images/enc_digits.bmp", "secret_key.txt");
    decrypt_file("images/enc_digits.bmp", "images/dec_digits.bmp", "secret_key.txt");

    // recognising patterns and removing non maxims
    recongnize_patterns(12, "images/dec_digits.bmp", "images/found_patterns.bmp","images/digit_templates/cifra0.bmp",
     "images/digit_templates/cifra1.bmp", "images/digit_templates/cifra2.bmp", "images/digit_templates/cifra3.bmp", 
     "images/digit_templates/cifra4.bmp", "images/digit_templates/cifra5.bmp", "images/digit_templates/cifra6.bmp",
     "images/digit_templates/cifra7.bmp", "images/digit_templates/cifra8.bmp" , "images/digit_templates/cifra9.bmp");
}
