"""
This file contains very ugly python code.
For debugging purpose only.
"""


import sys


class Node:
    def __init__(self, lst=[]):
        self.lst = lst

    def append(self, elem):
        self.lst.append(elem)

    def pop(self):
        return self.lst.pop()

    def __iter__(self):
        return iter(self.lst)


if __name__ == "__main__":
    infile = sys.argv[1]
    outfile = sys.argv[2]

    cell_list = {}
    net_list = {}

    with open(infile, "r") as f:
        content = f.readlines()

    balance = float(content[0])

    content = "".join(content[1:]).split(";")

    for line in content:
        line = line.split()

        if len(line) == 0:
            break

        assert line[0] == "NET"

        net_list[line[1]] = Node(list(set(line[2:])))
        for cell in net_list[line[1]]:
            assert cell != ";"

            if cell not in cell_list:
                cell_list[cell] = Node()
            cell_list[cell].append(line[1])

    print(len(cell_list), len(net_list))

    with open(outfile, "r") as f:
        content = f.readlines()

    g1 = set()
    g2 = set()

    cutsize = content[0].split()[-1]
    g1size = float(content[1].split()[-1])
    g2size = float(content[3].split()[-1])

    for elem in content[2].split():
        if elem == ";":
            break
        assert elem not in g1
        g1.add(elem)
    for elem in content[4].split():
        if elem == ";":
            break
        assert elem not in g2
        g2.add(elem)

    assert len(g1) == g1size
    assert len(g2) == g2size

    assert len(set(list(g1) + list(g2))) == len(g1) + len(g2)
    assert set(list(g1) + list(g2)) == set(cell_list)
    assert 0.5 - balance < (g1size / len(cell_list)) < 0.5 + balance
    assert g1size + g2size == len(cell_list)

    ccnt = 0
    for net in net_list.values():
        assert isinstance(net, Node)

        ctrue = cfalse = 0
        for c in net:
            if c in g1:
                ctrue += 1
            else:
                assert c in g2
                cfalse += 1
        if ctrue != 0 and cfalse != 0:
            ccnt += 1

    assert ccnt == int(cutsize)
