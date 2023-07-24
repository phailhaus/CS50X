#include <cs50.h>
#include <stdio.h>

int main(void)
{
    long cardnumber = get_long("%s", "Number: ");
    long workingnumber = cardnumber;
    int digits = 0; // Will hold the total number of digits
    int lastdigit; // Will hold the most recent digit
    int secondlastdigit; // Will hold the digit before that
    int checksum = 0; // Working sum

    while (workingnumber) // Will end when workingnumber hits zero
    {
        digits += 1;
        int thisdigit = workingnumber % 10; // Get the smallest digit
        secondlastdigit = lastdigit; // Push stored digit back one
        lastdigit = thisdigit; // Store new digit

        if (digits % 2 == 1) // If digit is odd
        {
            checksum += thisdigit; // Straight add to the checksum
        }
        else // If digit is even
        {
            checksum += ((thisdigit * 2) % 10) + ((thisdigit * 2) / 10); // Add the individual digits of thisdigit * 2
        }

        workingnumber /= 10; // Chop a digit off the end of workingnumber
    }

    if (checksum % 10 == 0)
    {
        if ((digits == 13 || digits == 16) && lastdigit == 4)
        {
            printf("%s", "VISA\n");
        }

        else if (digits == 15  && lastdigit == 3 && (secondlastdigit == 4 || secondlastdigit == 7))
        {
            printf("%s", "AMEX\n");
        }
        else if (digits == 16  && lastdigit == 5 && secondlastdigit >= 1 && secondlastdigit <= 5)
        {
            printf("%s", "MASTERCARD\n");
        }
        else
        {
            printf("%s", "INVALID\n");
        }

    }
    else
    {
        printf("%s", "INVALID\n");
    }

}