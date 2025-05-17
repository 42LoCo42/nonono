#!/usr/bin/env python3
from PIL import Image
from concurrent.futures import ThreadPoolExecutor
from sys import argv
import matplotlib.pyplot as plt
import numpy as np


def main():
    img = Image.open(argv[1])
    data = np.asarray(img)

    print(data.shape, data.dtype)

    _, axes = plt.subplots(2, 4, figsize=(80, 107))
    axes = axes.flatten()

    black = np.uint8(0)
    white = np.uint8(255)

    def bitplane(index):
        bp = np.where(data & (1 << index) > 0, white, black)

        out = Image.fromarray(bp)
        out.save(f"out{index}.png")
        print(f"saved bitplane {index}")

        axes[index].imshow(bp)
        axes[index].axis("off")
        axes[index].set_title(f"{index}", fontsize=12)

    with ThreadPoolExecutor() as executor:
        futures = [executor.submit(bitplane, i) for i in range(8)]

        for future in futures:
            future.result()

    plt.show()


if __name__ == "__main__":
    main()
