#include "helpers.h"

void colorize(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE color; // Set color pixels will be assigned
    color.rgbtRed = 0x7f;
    color.rgbtGreen = 0x30;
    color.rgbtBlue = 0xff;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            RGBTRIPLE target_color = image[y][x]; // Get existing pixels
            if (target_color.rgbtRed == 0x00 && target_color.rgbtGreen == 0x00 && target_color.rgbtBlue == 0x00) // Check if pixels are black
            {
                image[y][x] = color; // Set pixel to new color
            }
        }
    }
}
