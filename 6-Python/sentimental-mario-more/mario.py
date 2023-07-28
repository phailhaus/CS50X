height = 0
while height < 1 or height > 8:
    height = input("Height: ")
    try:
        height = int(height)
    except:
        height = 0

for i in range(height):
    blocks = i + 1
    printstring = " " * (height - blocks) + "#" * blocks + " " * 2 + "#" * blocks
    print(printstring)
