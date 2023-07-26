// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
} node;

// TODO: Choose number of buckets in hash table
const unsigned int N = 28;

typedef struct tree
{
    struct tree *next;
} tree;

tree *create_branch(void)
{
    tree *branch = malloc(sizeof(tree) * N);
    for (int i = 0; i < N; i++)
    {
        branch[i].next = NULL;
    }

    return branch;
}

// Hash table
// node *table[N];

tree *root;

// Word count in dictionary
int words = 0;

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    tree *position = root;
    char c = '0';
    int i = 0;
    while (c != '\0')
    {
        c = word[i];
        unsigned int c_hash = hash(&c);
        if (c_hash >= N) // Break if given a bad hash
        {
            printf("Hashing failed on character %c. Got hash %i\n", c, c_hash);
            return false;
        }

        if (position[c_hash].next == NULL) // If hash is uninitialized
        {
            return false; // Word is misspelled
        }

        position = position[c_hash].next; // Set position for the next loop

        i++;
    }

    return true; // Word is correctly spelled
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    // TODO: Improve this hash function
    char c = word[0];
    if (c == '\0')
    {
        return 0; // Null Terminator = hash 0
    }
    else if (c == '\'')
    {
        return 1; // Apostrophy = hash 1
    }
    else if (isalpha(c) != 0)
    {
        return toupper(c) - 63; // Letters = hash 2-27
    }
    else
    {
        return 99; // Error code - Not a hashable character
    }
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    root = create_branch(); // Initialize first layer of tree

    FILE *dictionary_file = fopen(dictionary, "r");
    char buffer[LENGTH + 2];

    while (fgets(buffer, sizeof(buffer), dictionary_file) != NULL)
    {
        int i = 0;
        char c = buffer[i];
        while (c != '\n' && c != '\0') // Find newline or terminator in word
        {
            i++;
            c = buffer[i];
        }

        tree *position = root;
        for (int j = 0; j <= i; j++) // Copy letters over to new string
        {
            unsigned int c_hash;
            if (j < i)
            {
                c_hash = hash(&buffer[j]);
            }
            else
            {
                c_hash = hash("\0");
            }

            if (c_hash >= N) // Break if given a bad hash
            {
                printf("Hashing failed on character %c. Got hash %i\n", buffer[j], c_hash);
                return false;
            }

            if (position[c_hash].next == NULL) // If hash is uninitialized
            {
                tree *branch = create_branch();

                position[c_hash].next = branch;
            }

            position = position[c_hash].next; // Set position for the next loop
        }

        words += 1;
    }
    fclose(dictionary_file);
    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    return words;
}

bool recursive_unloader(tree *branch)
{
    for (int i = 0; i < N; i++)
    {
        if (branch[i].next != NULL)
        {
            recursive_unloader(branch[i].next);
        }
    }
    free(branch);
    return true;
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    bool result = recursive_unloader(root);
    return result;
}
