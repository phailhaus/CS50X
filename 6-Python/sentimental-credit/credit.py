def main():
    cardnumber = input("Number: ")
    if check_sum(cardnumber):
        print(f"{check_number(cardnumber)}\n")
    else:
        print("INVALID\n")


def check_sum(cardnumber):
    checksum = 0
    even = True
    for i in range(len(cardnumber), 0, -1):
        try:
            thisdigit = int(cardnumber[i - 1])
        except:
            return False

        if even:  # Add the current digit
            checksum += thisdigit
        else:  # Add the individual digits of the current digit * 2
            checksum += int((thisdigit * 2) % 10)
            checksum += int((thisdigit * 2) / 10)
        even = not even

    if checksum % 10 == 0:
        return True
    else:
        return False


def check_number(cardnumber):
    number_length = len(cardnumber)
    if number_length in (13, 16) and int(cardnumber[0]) == 4:
        return "VISA"
    elif number_length == 15 and int(cardnumber[0:2]) in (34, 37):
        return "AMEX"
    elif number_length == 16 and 51 <= int(cardnumber[0:2]) <= 55:
        return "MASTERCARD"
    else:
        return "INVALID"


if __name__ == "__main__":
    main()
