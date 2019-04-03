#!/usr/bin/env python3
# pylint: disable=invalid-name, missing-docstring

import sys
from typing import Set

import matplotlib.pyplot as plt  # type: ignore
import matplotlib.ticker  # type: ignore
import numpy as np  # type: ignore
from matplotlib2tikz import save as tikz_save  # type: ignore

# process arguments
arg_set = set(sys.argv[1:])
EXPORT = bool({"-e", "--export"} & arg_set)
# EXPORT = False
arg_set -= {"-e", "--export"}
Y_LOG = bool({"-y", "-l", "--log-y"} & arg_set)
arg_set -= {"-y", "-l", "--log-y"}

if {"-h", "--help"} & arg_set:
    print(f"Usage: {sys.argv[0]} [-e|--export] [-y|--log-y]")
    sys.exit(0)


# load data
dtype = [("nodes", int), ("ranks", int), ("threads", int), ("io", float)]
data = np.loadtxt(
    "./outputs/runtimes.csv",
    dtype=dtype,
    delimiter=",",
    skiprows=1,
    usecols=(0, 1, 2, 4),
    max_rows=5,
)

xvals = data["nodes"] * data["ranks"]
plt.plot(xvals, data["io"], "o-", ms=4)

extra_params: Set[str] = set()

ax1 = plt.gca()
ax1.set_xscale("log", basex=2)
x_axis = ax1.get_xaxis()
x_axis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
x_axis.set_tick_params(which="minor", size=0)
x_axis.set_tick_params(which="minor", width=0)
ax1.set_xticks(xvals)
# use external package that doesn't round tick labels to 3 sig figs :S
extra_params |= {r"xticklabel={\xinttheiexpr[0]2^\tick\relax}"}

if Y_LOG:
    ax1.set_yscale("log")
    ax1.tick_params(which="both", labelright=True, right=True)
    # display as 10, 100, 1000 rather than 10^1, 10^2, 10^3, etc
    extra_params |= {"log ticks with fixed point"}

plt.xlabel("MPI ranks")
plt.ylabel("Time (s)")

if EXPORT:
    tikz_save(
        "io_comparison.tex",
        figureheight=r"\figureheight",
        figurewidth=r"\figurewidth",
        extra_axis_parameters=extra_params,
    )

plt.show()
