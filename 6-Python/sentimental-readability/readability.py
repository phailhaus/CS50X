import string


def main():
    text = input("Text: ")
    letters, words, sentences = count_string(text)
    grade = calculate_grade(letters, words, sentences)
    rounded_grade = round(grade)

    if grade < 1:
        print("Before Grade 1")
    elif grade >= 16:
        print("Grade 16+")
    else:
        print(f"Grade {rounded_grade}")


def count_string(text):
    letters = 0
    words = 1  # We are counting spaces so the first word will be missed
    sentences = 0

    for c in text:
        if c in string.ascii_letters:
            letters += 1
        elif c == " ":
            words += 1
        elif c in "!.?":
            sentences += 1

    return letters, words, sentences


def calculate_grade(letters, words, sentences):
    L = letters / words * 100  # Letters per 100 words
    S = sentences / words * 100  # Sentences per 100 words
    return 0.0588 * L - 0.296 * S - 15.8


if __name__ == "__main__":
    main()
