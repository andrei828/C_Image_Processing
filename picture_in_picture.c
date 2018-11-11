#include <stdio.h>

int main()
{
    FILE * in_big = fopen("image.bmp", "rb");
    FILE * in_small = fopen("small_image.bmp", "rb");
    FILE * out_bin = fopen("picture_in_picture.bmp", "wb");

    if (in_big == NULL || in_small == NULL || out_bin == NULL)
    {
        printf("Could not open files");
        return 0;
    }

    // variables for pixel manipulation
    int x, y;

    // copy header into new picture
    for (int i = 0; i < 54; i++)
    {
        fread(&x, 1, 1, in_big);
        fread(&y, 1, 1, in_small);
        fwrite(&x, 1, 1, out_bin);
    }

    // picture in picture
    int i = 0;
    while (fread(&x, 1, 1, in_big) == 1)
    {
        if (i == 2400) i = 0;
        
        if (i < 300)
            if (fread(&y, 1, 1, in_small) == 1)
                fwrite(&y, 1, 1, out_bin);
            else
            {
                fwrite(&x, 1, 1, out_bin);
                break;
            }
        else
            fwrite(&x, 1, 1, out_bin);
        
        i++;
    }  

    // finish the image
    while (fread(&x, 1, 1, in_big) == 1)
        fwrite(&x, 1, 1, out_bin);

    fclose(in_big);
    fclose(in_small);
    fclose(out_bin);
    return 0;
}