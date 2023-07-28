import csv
import sys


def main():
    # Ensure correct usage
    if len(sys.argv) != 3:
        sys.exit("Usage: python dna.py data.csv sequence.txt")

    # Read database file into a variable
    people = list()
    with open(sys.argv[1], newline="") as csvfile:
        filedict = csv.DictReader(csvfile, delimiter=",", quotechar='"')
        for filerow in filedict:
            for key in filerow.keys():
                try:
                    filerow[key] = int(filerow[key])
                except:
                    pass
            people.append(filerow)

    # Create list of target STRs
    target_strs = list(people[0].keys())
    target_strs.remove("name")

    # Read DNA sequence file into a variable
    with open(sys.argv[2], newline="") as textfile:
        sequence = textfile.read()
    sequence = sequence[:-1]  # Remove \n at the end

    # Find longest match of each STR in DNA sequence
    results = dict()
    for subsequence in target_strs:
        results[subsequence] = longest_match(sequence, subsequence)

    # Check database for matching profiles
    matched = False
    target_count = len(target_strs)

    for person in people:
        matches = 0
        for key in results.keys():
            if person[key] == results[key]:
                matches += 1
            else:
                break
        if matches == target_count:
            matched = True
            print(person["name"])
            break
    if not matched:
        print("No match")


def longest_match(sequence, subsequence):
    """Returns length of longest run of subsequence in sequence."""

    # Initialize variables
    longest_run = 0
    subsequence_length = len(subsequence)
    sequence_length = len(sequence)

    # Check each character in sequence for most consecutive runs of subsequence
    for i in range(sequence_length):
        # Initialize count of consecutive runs
        count = 0

        # Check for a subsequence match in a "substring" (a subset of characters) within sequence
        # If a match, move substring to next potential match in sequence
        # Continue moving substring and checking for matches until out of consecutive matches
        while True:
            # Adjust substring start and end
            start = i + count * subsequence_length
            end = start + subsequence_length

            # If there is a match in the substring
            if sequence[start:end] == subsequence:
                count += 1

            # If there is no match in the substring
            else:
                break

        # Update most consecutive matches found
        longest_run = max(longest_run, count)

    # After checking for runs at each character in seqeuence, return longest run found
    return longest_run


main()
