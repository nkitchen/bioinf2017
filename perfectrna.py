#!/usr/bin/python3

import fileinput
import sys

s = next(fileinput.input()).strip()

complement = {
    'A': 'U',
    'U': 'A',
    'C': 'G',
    'G': 'C',
}

# Grammar for perfect strings:
# P ::= B B'
#     | B P B'
#     | P P

# Contains (i, j) if s[i:j] is perfect
perfect = set()

# Seed with B B'.
for i in range(0, len(s) - 1):
    j = i + 1
    if s[j] == complement[s[i]]:
        perfect.add((i, i + 2))

# For each possible length...
for n in range(4, len(s) + 1, 2):
    # Check for B P B' of length n.
    # => P has length n - 2.
    for i in range(0, len(s) - n + 1):
        if (s[i] == complement[s[i + n - 1]] and
            (i + 1, i + n - 1) in perfect):
            perfect.add((i, i + n))

    # Check for P P of length n.
    for i in range(0, len(s) - n + 1):
        for k in range(2, n - 1, 2):
            # First P has length k.
            # Second P has length n - k.
            if ((i, i + k) in perfect and
                (i + k, i + n) in perfect):
                perfect.add((i, i + n))

if len(s) % 2 == 0:
    if (0, len(s)) in perfect:
        print("perfect")
    else:
        print("imperfect")
    sys.exit(0)

# Grammar for almost perfect strings:
# A ::= B C B'
#     | B A B'
#     | C P
#     | P C
#     | A P
#     | P A
#
# The code is simpler if we treat single bases as almost perfect,
# so that a lone C is an A.

# Seed with single bases.
almostPerfect = set((i, i + 1) for i in range(0, len(s) - 1))

# For each possible length...
for n in range(3, len(s) + 1, 2):
    # Check for B A B' of length n.
    for i in range(0, len(s) - n + 1):
        if (s[i] == complement[s[i + n - 1]] and
            (i + 1, i + n - 1) in almostPerfect):
            almostPerfect.add((i, i + n))

    for i in range(0, len(s) - n + 1):
        # Check for A P of length n.
        for k in range(1, n - 1, 2):
            # A has length k.
            # P has length n - k.
            if ((i, i + k) in almostPerfect and
                (i + k, i + n) in perfect):
                almostPerfect.add((i, i + n))

        # Check for P A of length n.
        for k in range(2, n, 2):
            # P has length k.
            # A has length n - k.
            if ((i, i + k) in perfect and
                (i + k, i + n) in almostPerfect):
                almostPerfect.add((i, i + n))

if (0, len(s)) in almostPerfect:
    print("almost perfect")
else:
    print("imperfect")

