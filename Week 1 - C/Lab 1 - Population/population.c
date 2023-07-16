#include <cs50.h>
#include <stdio.h>

int get_int_bounded(string message, bool lowerboundbool, int lowerbound, bool upperboundbool, int upperbound);

int main(void)
{
    // Prompt for start size
    int start = get_int_bounded("Starting Llama count: ", true, 9, false, 0);

    // Prompt for end size
    int end = get_int_bounded("Ending Llama count: ", true, start, false, 0);

    // Calculate number of years until we reach threshold
    int population = start;
    int years = 0;
    if (start != end)
    {
        do
        {
            population = population + (population / 3) - (population / 4);
            years++;
        }
        while (population < end);
    }

    // Print number of years
    printf("Years: %i\n", years);
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