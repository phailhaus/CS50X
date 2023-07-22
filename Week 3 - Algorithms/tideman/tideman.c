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
            printf("%s vs %s - %i\n", candidates[i], candidates[j], matchup_score);
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

    while(true)
    {
        sorted = true;
        for (int i = 0; i < pair_count - loops - 1; i++)
        {
            pair pair_1 = pairs[i];
            int matchup_score_1 = preferences[pair_1.winner][pair_1.loser] - preferences[pair_1.loser][pair_1.winner];

            pair pair_2 = pairs[i+1];
            int matchup_score_2 = preferences[pair_2.winner][pair_2.loser] - preferences[pair_2.loser][pair_2.winner];

            if (matchup_score_1 < matchup_score_2)
            {
                sorted = false;
                pairs[i] = pair_2;
                pairs[i+1] = pair_1;
            }
        }

        if (sorted)
        {
            for (int i = 0; i < pair_count; i++)
            {
                printf("%s beats %s\n", candidates[pairs[i].winner], candidates[pairs[i].loser]);
            }
            return;
        }

        loops += 1;
    }

    return;
}

// Recursively check earlier pairs to see if current loser previously beat the current winner
bool chain_check(int pairs_position, int target_candidate)
{
    printf("Checking to see if pair %i has %s lose\n", pairs_position, candidates[target_candidate]);
    if (pairs[pairs_position].loser == target_candidate) // If the winner we were checking lost to somebody down the chain from its loser, send up true
    {
        return true;
    }

    for (int i = pairs_position - 1; i >= 0; i--) // Check every pair stronger than this
    {
        if (pairs[i].winner == pairs[pairs_position].loser && locked[pairs[i].winner][pairs[i].loser] == true) // If that pair has the loser of this one win and has itself been locked, check it
        {
            if (chain_check(i, target_candidate))
            {
                return true;
            }
        }
    }

    return false;
}


// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        printf("Checking pair %i\n", i);
        if (!chain_check(i, pairs[i].winner))
        {
            printf("Pair %i locked\n", i);
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }

    return;
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