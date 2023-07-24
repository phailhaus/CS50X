#include <math.h>

#include "helpers.h"

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            RGBTRIPLE color = image[y][x]; // Get existing pixels
            int colorsum = color.rgbtRed + color.rgbtGreen + color.rgbtBlue;
            colorsum = round(colorsum / 3.0); // Calculate the average. Uses 3.0 to force float math
            color.rgbtRed = colorsum;
            color.rgbtGreen = colorsum;
            color.rgbtBlue = colorsum;
            image[y][x] = color; // Set pixel to new color
        }
    }
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE image_out[height][width];
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            image_out[y][width - x - 1] = image[y][x]; // Copy existing pixels to new position in temp image
        }
    }

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            image[y][x] = image_out[y][x]; // Overwrite original image
        }
    }
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE image_out[height][width];
    for (int y = 0; y < height; y++)
    {
        int starting_y = 0; // Set y coord to start with. Protects against undershooting the array and wraparounds
        if (y != 0)
        {
            starting_y = y - 1;
        }

        for (int x = 0; x < width; x++)
        {
            int r_sum = 0;
            int g_sum = 0;
            int b_sum = 0;
            float pixel_count = 0; // This is a float to force floating point math later when calculating average

            int starting_x = 0; // Set x coord to start with. Protects against undershooting the array and wraparounds
            if (x != 0)
            {
                starting_x = x - 1;
            }

            for (int y2 = starting_y; y2 <= y + 1 && y2 < height; y2++)
                // Start with the column determined above, go until y + 1 or edge of image
            {
                for (int x2 = starting_x; x2 <= x + 1 && x2 < width; x2++)
                    // Start with the row determined above, go until y + 1 or edge of image
                {
                    r_sum += image[y2][x2].rgbtRed;
                    g_sum += image[y2][x2].rgbtGreen;
                    b_sum += image[y2][x2].rgbtBlue;
                    pixel_count++;
                }
            }

            image_out[y][x].rgbtRed = round(r_sum / pixel_count); // Write avg color
            image_out[y][x].rgbtGreen = round(g_sum / pixel_count);
            image_out[y][x].rgbtBlue = round(b_sum / pixel_count);
        }
    }

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            image[y][x] = image_out[y][x]; // Overwrite original image
        }
    }
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE image_out[height][width];
    for (int y = 0; y < height; y++)
    {
        int starting_y = 0; // Set y coord to start with. Protects against undershooting the array and wraparounds
        if (y != 0)
        {
            starting_y = y - 1;
        }

        for (int x = 0; x < width; x++)
        {
            float r_Gx = 0; // These are floats to force floating point math later
            float g_Gx = 0;
            float b_Gx = 0;

            float r_Gy = 0;
            float g_Gy = 0;
            float b_Gy = 0;

            int starting_x = 0; // Set x coord to start with. Protects against undershooting the array and wraparounds
            if (x != 0)
            {
                starting_x = x - 1;
            }

            for (int y2 = starting_y; y2 <= y + 1 && y2 < height; y2++)
                // Start with the column determined above, go until y + 1 or edge of image
            {
                for (int x2 = starting_x; x2 <= x + 1 && x2 < width; x2++)
                    // Start with the row determined above, go until y + 1 or edge of image
                {
                    float mult_Gx = 0; // Determine Gx multiplier for this target pixel
                    if (x2 < x)
                    {
                        mult_Gx = -1;
                    }
                    else if (x2 > x)
                    {
                        mult_Gx = 1;
                    }
                    if (y2 == y)
                    {
                        mult_Gx = mult_Gx * 2;
                    }

                    float mult_Gy = 0; // Determine Gy multiplier for this target pixel
                    if (y2 < y)
                    {
                        mult_Gy = -1;
                    }
                    else if (y2 > y)
                    {
                        mult_Gy = 1;
                    }
                    if (x2 == x)
                    {
                        mult_Gy = mult_Gy * 2;
                    }

                    r_Gx += image[y2][x2].rgbtRed * mult_Gx;
                    g_Gx += image[y2][x2].rgbtGreen * mult_Gx;
                    b_Gx += image[y2][x2].rgbtBlue * mult_Gx;

                    r_Gy += image[y2][x2].rgbtRed * mult_Gy;
                    g_Gy += image[y2][x2].rgbtGreen * mult_Gy;
                    b_Gy += image[y2][x2].rgbtBlue * mult_Gy;
                }
            }

            int out_red = round(sqrtf(r_Gx * r_Gx + r_Gy * r_Gy));
            if (out_red > 255)
            {
                out_red = 255;
            }
            int out_green = round(sqrtf(g_Gx * g_Gx + g_Gy * g_Gy));
            if (out_green > 255)
            {
                out_green = 255;
            }
            int out_blue = round(sqrtf(b_Gx * b_Gx + b_Gy * b_Gy));
            if (out_blue > 255)
            {
                out_blue = 255;
            }

            image_out[y][x].rgbtRed = out_red;
            image_out[y][x].rgbtGreen = out_green;
            image_out[y][x].rgbtBlue = out_blue;
        }
    }

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            image[y][x] = image_out[y][x]; // Overwrite original image
        }
    }
}
