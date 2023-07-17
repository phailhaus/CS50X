#include <ctype.h>
#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Points assigned to each letter of the alphabet
int POINTS[] = {1, 3, 3, 2, 1, 4, 2, 4, 1, 8, 5, 1, 3, 1, 1, 3, 10, 1, 1, 1, 1, 4, 4, 8, 4, 10};

int compute_score(string word);

int main(void)
{
    // Get input words from both players
    string word1 = get_string("Player 1: ");
    string word2 = get_string("Player 2: ");

    // Score both words
    int score1 = compute_score(word1);
    int score2 = compute_score(word2);

    // Print the winner
    string output;
    if (score1 == score2)
    {
        output = "Tie!";
    }
    else if (score1 > score2)
    {
        output = "Player 1 wins!";
    }
    else
    {
        output = "Player 2 wins!";
    }
    printf("%s\n", output);
}

int compute_score(string word)
{
    // Compute and return score for string
    int length = strlen(word);
    int score = 0;

    for (int i = 0; i < length; i++)
    {
        char c = word[i];
        int charid = toupper(c);
        if (charid >= 65 && charid <= 90) // A is ASCII ID 65, Z is 90.
        {
            score += POINTS[charid - 65]; // Get letter value from POINTS array and add to word score.
        }
    }
    return score;
}
