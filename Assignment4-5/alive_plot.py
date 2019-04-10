#!/usr/bin/env python3
# pylint: disable=invalid-name, missing-docstring

import matplotlib.pyplot as plt  # type: ignore
import numpy as np  # type: ignore
from matplotlib2tikz import save as tikz_save  # type: ignore

EXPORT = True

# load data
ticks, *data = np.loadtxt(
    "./outputs/alive_over_time.csv", delimiter=",", skiprows=1, unpack=True
)

rank_counts = [128, 256, 512, 2048, 8192]

for ranks, col in zip(rank_counts, data):
    plt.plot(ticks, col / 1e6, label=f"{ranks} ranks")

plt.xlabel("Simulation time (ticks)")
plt.ylabel("Alive cells (millions)")
plt.legend()

tikz_save(
    "latex/alive_cells.tex", figureheight=r"\figureheight", figurewidth=r"\figurewidth"
)

plt.show()
