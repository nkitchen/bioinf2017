import sys

input = sys.stdin.read()

words = input.split()

s = words[0]
t = words[1]

locs = []
i = 0
while i < len(s):
    j = s.find(t, i)
    if j < 0:
        break
    locs.append(j)
    i = j + 1

print(" ".join(str(loc + 1) for loc in locs))
