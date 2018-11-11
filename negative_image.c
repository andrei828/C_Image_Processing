#include <stdio.h>

int main()
{
    FILE * in = fopen("image.bmp", "rb");
    FILE * out = fopen("modified_image.bmp", "wb");

    if (in == NULL || out == NULL)
    {
        printf("Files could not open");
        return 0;
    }

    int x, y;
    for (int i = 0; i < 54; i++)
    {
        fread(&x, 1, 1, in);
        fwrite(&x, 1, 1, out);
    }

    while (fread(&x, 1, 1, in) == 1)
    {
        y = 255 - x;
        fwrite(&y, 1, 1, out);
    }

    fclose(in);
    fclose(out);
}

