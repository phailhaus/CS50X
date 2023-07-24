#include <cs50.h>
#include <math.h>
#include <stdio.h>

int count_string(string text, int counts[3]);

int main(void)
{
    string text = get_string("Text: ");

    int counts[3];
    count_string(text, counts);
    int letters = counts[0];
    int words = counts[1];
    int sentences = counts[2];

    float L = letters * 100.0 / words; // Letters per 100 words. Math must be done in this order to make it do float division
    float S = sentences * 100.0 / words; // Sentences per 100 words. Math must be done in this order to make it do float division
    float index = 0.0588 * L - 0.296 * S - 15.8;
    int roundindex = round(index);

    if (index >= 16)
    {
        printf("%s\n", "Grade 16+");
    }
    else if (index < 1)
    {
        printf("%s\n", "Before Grade 1");
    }
    else
    {
        printf("Grade %i\n", roundindex);
    }

}

int count_string(string text, int counts[3])
{
    int i = 0;
    int charid = 1;
    int lettercount = 0;
    int wordcount = 0;
    int sentencecount = 0;

    do
    {
        charid = text[i];
        if ((charid >= 65 && charid <= 90) || (charid >= 97 && charid <= 122)) // Is the character a letter
        {
            lettercount += 1;
        }
        else if (charid == 32) // Is the character a space
        {
            wordcount += 1;
        }
        else if (charid == 33 || charid == 46 || charid == 63) // Is the character !.?
        {
            sentencecount += 1;
        }

        i++;
    }
    while (charid != 0);

    wordcount += 1; // Count the final word

    counts[0] = lettercount;
    counts[1] = wordcount;
    counts[2] = sentencecount;

    return 0;
}