#include <cs50.h>
#include <stdio.h>

int get_int_bounded(string message, bool lowerboundbool, int lowerbound, bool upperboundbool, int upperbound);
void printchars(char symbol, int length);

int main(void)
{
    // Establish characters
    char newline = '\n';
    char space = ' ';
    char block = '#';

    // Get input from user of pyramid height
    int height = get_int_bounded("Pyramid Height: ", true, 1, true, 8);

    // Print Pyramid

    for (int i = 0; i < height; i++)
    {
        if (i < height - 1)
        {
            printchars(space, height - i - 1); // Draw spaces before pyramid block
        }

        printchars(block, i + 1); // Draw left blocks
        printchars(space, 2); // Draw gap
        printchars(block, i + 1); // Draw right blocks
        printchars(newline, 1); // Move to next line
    }


}

int get_int_bounded(string message, bool lowerboundbool, int lowerbound, bool upperboundbool, int upperbound)
{
    int i = 0;
    do
    {
        i = get_int("%s", message);
    }
    while (!((!lowerboundbool || i >= lowerbound) && (!upperboundbool || i <= upperbound)));

    return i;
}

void printchars(char symbol, int length)
{
    for (int i = 0; i < length; i++)
    {
        printf("%c", symbol);
    }

}