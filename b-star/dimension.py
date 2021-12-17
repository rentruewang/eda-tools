import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("infile")
    parser.add_argument("outfile")
    flags = parser.parse_args()

    with open(flags.infile, "r") as f:
        lines = tuple(filter(None, (l.strip() for l in f.readlines())))

    blocks = int(lines[1].split()[1])

    print(flags.infile, flags.outfile)

    start = 3
    dimensions = {}

    for i in range(start, start + blocks):
        (name, w, h) = lines[i].split()
        dimensions[name] = (int(w), int(h))

    # print(dimensions)
    
    with open(flags.outfile, "r") as f:
        lines = f.readlines()
        area = int(lines[2])
        (W, H) = map(int, lines[3].split())
        assert area == W * H

        data = tuple(filter(None, lines[5:]))

        for line in data:
            (name, x1, y1, x2, y2) = line.split()
            (x1, y1, x2, y2) = map(int, (x1, y1, x2, y2))

            w = x2 - x1
            h = y2 - y1

            entry = dimensions[name]
            # print(name, entry, w, h)
            assert entry == (w, h) or entry == (h, w)
        assert len(dimensions) == len(data)
