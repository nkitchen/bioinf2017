#!/usr/bin/python3

import collections
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

# perfectEnds[i] contains j if s[i:j] is perfect.
# perfectStarts[j] contains i if s[i:j] is perfect.
perfectEnds = collections.defaultdict(set)
perfectStarts = collections.defaultdict(set)

queue = collections.OrderedDict()

# Seed with B B'.
for i in range(0, len(s) - 1):
    if s[i + 1] == complement[s[i]]:
        j = i + 2
        perfectEnds[i].add(j)
        perfectStarts[j].add(i)
        queue[(i, j)] = True

while queue:
    (i, j), _ = queue.popitem(last=False)

    # Try expanding with B P B'.
    if 0 < i and j < len(s):
        if s[i - 1] == complement[s[j]]:
            ii = i - 1
            jj = j + 1
            perfectEnds[ii].add(jj)
            perfectStarts[jj].add(ii)
            queue[(ii, jj)] = True

    # Try expanding with P P, where this is the first P.
    for k in perfectEnds[j]:
        perfectEnds[i].add(k)
        perfectStarts[k].add(i)
        queue[(i, k)] = True

    # Try expanding with P P, where this is the second P.
    for h in perfectStarts[i]:
        perfectStarts[j].add(h)
        perfectEnds[h].add(j)
        queue[(h, j)] = True

if len(s) % 2 == 0:
    if len(s) in perfectEnds[0]:
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

almostPerfectEnds = collections.defaultdict(set)
almostPerfectStarts = collections.defaultdict(set)

for i in range(0, len(s)):
    almostPerfectEnds[i].add(i + 1)
    almostPerfectStarts[i + 1].add(i)
    queue[(i, i + 1)] = True

while queue:
    (i, j), _ = queue.popitem(last=False)

    # Try expanding with B A B'.
    if 0 < i and j < len(s):
        if s[i - 1] == complement[s[j]]:
            ii = i - 1
            jj = j + 1
            almostPerfectEnds[ii].add(jj)
            almostPerfectStarts[jj].add(ii)
            queue[(ii, jj)] = True

    # Try expanding with A P.
    for k in perfectEnds[j]:
        almostPerfectEnds[i].add(k)
        almostPerfectStarts[k].add(i)
        queue[(i, k)] = True

    # Try expanding with P A.
    for h in perfectStarts[i]:
        almostPerfectStarts[j].add(h)
        almostPerfectEnds[h].add(j)
        queue[(h, j)] = True

if len(s) in almostPerfectEnds[0]:
    print("almost perfect")
else:
    print("imperfect")

# vim: set shiftwidth=4 :

