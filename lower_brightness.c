#include <stdio.h>

void lower_brightness(const char * name);

int main()
{
    const char * name = "small_image.bmp";
    lower_brightness(name);
    return 0;
}

void lower_brightness(const char * name)
{
    FILE * in = fopen("small_image.bmp", "rb");
    FILE * out = fopen("modified_small_image.bmp", "wb");

    if (in == NULL || out == NULL)
    {
        printf("Files could not open");
        return;
    }

    int x, y;

    // copy header of bmp file
    for (int i = 0; i < 54; i++)
    {
        fread(&x, 1, 1, in);
        fwrite(&x, 1, 1, out);
    }

    //copy image with lower brightness
    while (fread(&x, 1, 1, in) == 1)
    {
        y = x - 50;
        if (y < 0) y = 0;
        fwrite(&y, 1, 1, out);
    }

    fclose(in);
    fclose(out);
}