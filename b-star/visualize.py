import argparse

import numpy as np
from matplotlib import pyplot as plt

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("file")
    parser.add_argument("output")
    flags = parser.parse_args()

    file = flags.file
    output = flags.output

    print(file, output)

    with open(file, "r") as f:
        lines = f.readlines()
        area = int(lines[2])
        (W, H) = map(int, lines[3].split())
        assert area == W * H

        # print(W, H)

        grid = np.ones((W, H))

        for line in lines[5:]:
            (name, x1, y1, x2, y2) = line.split()
            (x1, y1, x2, y2) = map(int, (x1, y1, x2, y2))

            # print(x1, y1, x2, y2)

            assert 0 <= x1 <= x2 <= W
            assert 0 <= y1 <= y2 <= H

            for i in range(x1, x2):
                grid[i, y1] = 0
                grid[i, y2 - 1] = 0

            for i in range(y1, y2):
                grid[x1, i] = 0
                grid[x2 - 1, i] = 0

    plt.figure(figsize=(20, 18))
    plt.imshow(grid)
    plt.savefig(output)
