import fileinput

s = 0
for line in fileinput.input():
    for w in line.split():
        s += int(w)
print(s)
