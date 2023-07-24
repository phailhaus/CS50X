#include <cs50.h>
#include <string.h>
#include <stdio.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }

    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count - 1; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]] += 1;
        }
    }

    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count - 1; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            int matchup_score = preferences[i][j] - preferences[j][i];
            if (matchup_score > 0)
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
            }
            else if (matchup_score < 0)
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
            }

            if (matchup_score != 0)
            {
                pair_count += 1;
            }
        }
    }

    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    bool sorted = false;
    int loops = 0;

    while (true)
    {
        sorted = true;
        for (int i = 0; i < pair_count - loops - 1; i++)
        {
            pair pair_1 = pairs[i];
            int matchup_score_1 = preferences[pair_1.winner][pair_1.loser] - preferences[pair_1.loser][pair_1.winner];

            pair pair_2 = pairs[i + 1];
            int matchup_score_2 = preferences[pair_2.winner][pair_2.loser] - preferences[pair_2.loser][pair_2.winner];

            if (matchup_score_1 < matchup_score_2)
            {
                sorted = false;
                pairs[i] = pair_2;
                pairs[i + 1] = pair_1;
            }
        }

        if (sorted)
        {
            return;
        }

        loops += 1;
    }

    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++) // Work through all the pairs from strongest to weakest
    {
        bool been_beaten[candidate_count]; // Initialize an array that will be used when tracing chains
        for (int j = 0; j < candidate_count; j++)
        {
            been_beaten[j] = false;
        }

        if (i > 0)
        {
            bool has_won = false;
            bool has_lost = false;
            // Optimization that skips the chain check
            // if the loser has never won or the winner has never lost in stronger matchups
            for (int j = i - 1; j >= 0; j--)
            {
                if (locked[pairs[j].winner][pairs[j].loser] == true && pairs[j].loser == pairs[i].winner)
                {
                    has_lost = true;
                }
                if (locked[pairs[j].winner][pairs[j].loser] == true && pairs[j].winner == pairs[i].loser)
                {
                    has_won = true;
                }
            }

            if (has_won == true && has_lost == true)
            {
                // Chains can't be longer than the number of candidates
                // so do that number of passes through the pairs
                for (int j = 0; j < candidate_count; j++)
                {
                    for (int k = i - 1; k >= 0; k--) // Check every pair prior to the one that is in the process of being locked
                    {
                        // Check if the pair is locked and the winner is either the loser we are looking for
                        // or someone who has been beaten by them in a prior check
                        if (locked[pairs[j].winner][pairs[j].loser] == true && (pairs[k].winner == pairs[i].loser || been_beaten[pairs[k].winner] == true))
                        {
                            been_beaten[pairs[k].loser] = true;
                        }
                    }
                }
            }
        }

        if (been_beaten[pairs[i].winner] == false)
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
}

// Print the winner of the election
void print_winner(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        bool winner = false;
        bool loser = false;

        for (int j = 0; j < candidate_count && loser == false; j++)
        {
            if (locked[i][j] == true)
            {
                winner = true;
            }
            else if (locked[j][i] == true)
            {
                loser = true;
            }
        }

        if (winner == true && loser == false)
        {
            printf("%s\n", candidates[i]);
        }
    }
    return;
}