#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{       
    char option[7]; 
BEGIN:
    printf("Choose operating system:\n1) Windows\n2) Linux\n>");
    scanf("%7s", option);

    if (!strcmp(option, "Windows") || !strcmp(option, "windows") || !strcmp(option, "1"))
    {
        system("gcc criptare.c -o criptare.exe && criptare.exe");
        system("gcc patternuri.c -o patternuri.exe && patternuri.exe");
    }
    else if (!strcmp(option, "Linux") || !strcmp(option, "linux") || !strcmp(option, "2"))
    {
        system("gcc criptare.c -o criptare.out && ./criptare.out");
        system("gcc patternuri.c -o patternuri.out && ./patternuri.out");
    }
    else
    {
        printf("Please select a valid option\n");
        goto BEGIN;
    }
}
