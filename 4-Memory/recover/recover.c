#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    if (argc != 2) // Reject wrong numbers of arguments
    {
        printf("%s", "Usage: ./recover IMAGE\n");
        return 1;
    }

    FILE *raw_file = fopen(argv[1], "r"); // Open file
    if (raw_file == NULL) // Error if file failed to open
    {
        printf("Could not open %s.\n", argv[1]);
        return 1;
    }

    int in_file = 0;
    int file_num = 0;
    char out_name[8];
    FILE *out_ptr = NULL;

    int BLOCK_SIZE = 512;
    BYTE buffer[BLOCK_SIZE];
    while (fread(buffer, 1, BLOCK_SIZE, raw_file) == BLOCK_SIZE)
    {
        if
        (
            buffer[0] == 0xff &&
            buffer[1] == 0xd8 &&
            buffer[2] == 0xff &&
            buffer[3] >= 0xe0 &&
            buffer[3] <= 0xef
        ) // Indicates the start of a JPEG
        {
            printf("%s", "Match found\n");
            if (in_file == 0)
            {
                in_file = 1; // Flag noting first-time setup is done
                sprintf(out_name, "%03i.jpg", file_num);
                printf("Starting %03i.jpg\n", file_num);
                out_ptr = fopen(out_name, "w"); // Open the first file
            }
            else
            {
                fclose(out_ptr); // Close the open file
                file_num++;
                sprintf(out_name, "%03i.jpg", file_num);
                printf("Starting %03i.jpg\n", file_num);
                out_ptr = fopen(out_name, "w"); // Open the next file
            }
        }

        if (in_file == 1)
        {
            fwrite(buffer, sizeof(BYTE), BLOCK_SIZE, out_ptr); // Write file
        }
    }
    fclose(out_ptr); // Close the last file
    fclose(raw_file);
}