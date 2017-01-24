import sys

initial = next(sys.stdin).split()

reactions = {}
for line in sys.stdin:
    pre, post = line.strip().split("->")
    reagents = pre.split("+")
    products = post.split("+")
    reaction = (set(reagents), set(products))
    for r in reagents:
        reactions.setdefault(r, []).append(reaction)

present = set(initial)
q = list(initial)
while q:
    r = q.pop(0)
    newProducts = set()
    for reaction in reactions.get(r, []):
        if reaction[0] <= present:
            newProducts |= reaction[1] - present

    present |= newProducts

    for p in newProducts:
        q.append(p)

print(" ".join(sorted(present)))
