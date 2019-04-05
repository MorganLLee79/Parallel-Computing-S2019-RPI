#!/usr/bin/env python3
# pylint: disable=invalid-name, missing-docstring

import glob

import matplotlib.pyplot as plt  # type: ignore
import numpy as np  # type: ignore
from matplotlib2tikz import save as tikz_save  # type: ignore

for filename in glob.glob("outputs/2-2_heatmap-*thresh"):
    # load the heatmap file as a flat array of 2-byte big-endian integers,
    # then wrap to a 1K×1K 2D array
    heatmap = np.fromfile(filename, ">i2").reshape(1024, 1024)
    # ax = plt.gca()
    # ax.tick_params(which="both", direction="in", top=True, right=True)

    plt.pcolormesh(heatmap, vmin=0, vmax=1024)
    cbar = plt.colorbar()
    cbar.ax.set_ylabel("alive cells")
    tikz_save(
        "latex/{}.tex".format(filename[12:]),
        figureheight=r"\figureheight",
        figurewidth=r"\figurewidth",
    )
    plt.show()
