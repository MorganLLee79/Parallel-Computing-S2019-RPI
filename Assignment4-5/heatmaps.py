#!/usr/bin/env python3
# pylint: disable=invalid-name, missing-docstring

import matplotlib.pyplot as plt  # type: ignore
import numpy as np  # type: ignore
from matplotlib2tikz import save as tikz_save  # type: ignore

fig, axes = plt.subplots(nrows=2, ncols=2, sharex="all", sharey="all")
# fig, axes = plt.subplots(nrows=2, ncols=2)
for i, ax in enumerate(axes.flat):
    filename = "outputs/2-2_heatmap-{}thresh".format(i * 25)
    # load the heatmap file as a flat array of 2-byte big-endian integers,
    # then wrap to a 1K×1K 2D array
    heatmap = np.fromfile(filename, ">i2").reshape(1024, 1024)
    # ax = plt.gca()
    # ax.tick_params(which="both", direction="in", top=True, right=True)

    mesh = ax.pcolormesh(heatmap, vmin=0, vmax=1024)
    ax.set_title("threshold = {:.2f}".format(0.25 * i))
    print(
        "{}: {:.0f} cells ({:.0f}\\%), stdev={:.1f} ({:.1f}\\%)".format(
            0.25 * i,
            heatmap.mean(),
            heatmap.mean() / 1024 * 100,
            heatmap.std(),
            heatmap.std() / 1024 * 100,
        )
    )

fig.tight_layout()

cbar = fig.colorbar(mesh, ax=axes.ravel().tolist())
cbar.set_label("alive cells")

# Don't run this, I had to modify the generated tikz code by hand.
# tikz_save(
#     "latex/heatmap.tex",
#     figureheight=r"0.4\figureheight",
#     figurewidth=r"0.4\figurewidth",
#     show_info=True,
# )

plt.show()
