#!/usr/bin/env python3
# pylint: disable=invalid-name, missing-docstring

import sys
from typing import Set

import matplotlib.pyplot as plt  # type: ignore
import matplotlib.ticker  # type: ignore
import numpy as np  # type: ignore
import numpy_indexed as npi  # type: ignore
from matplotlib2tikz import save as tikz_save  # type: ignore

# process arguments
arg_set = set(sys.argv[1:])
EXPORT = bool({"-e", "--export"} & arg_set)
# EXPORT = False
arg_set -= {"-e", "--export"}
# # Y_LOG = bool({"-y", "-l", "--log-y"} & arg_set)
# Y_LOG = True
# arg_set -= {"-y", "-l", "--log-y"}

if {"-h", "--help"} & arg_set:
    print(f"Usage: {sys.argv[0]} [-e|--export] [-y|--log-y]")
    sys.exit(0)


# load data
dtype = [("nodes", int), ("ranks", int), ("threads", int), ("runtime", float)]
data = np.loadtxt("./outputs/runtimes.csv", dtype=dtype, delimiter=",", skiprows=1)

grouped = npi.group_by(data["threads"]).split(data)

rank_vals: Set[int] = set()

baseline = data["runtime"][15] * data["nodes"][15] * 64

for grp in grouped:
    threads = grp["threads"][0]
    label = "{} thread{} per rank".format(threads, "s" if threads > 1 else "")
    xvals = grp["nodes"]
    plt.plot(
        xvals, baseline / (grp["runtime"] * grp["nodes"] * 64), "o-", ms=4, label=label
    )
    rank_vals |= set(xvals)

extra_params: Set[str] = set()

ax1 = plt.gca()
ax1.set_xscale("log", basex=2)
x_axis = ax1.get_xaxis()
x_axis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
x_axis.set_tick_params(which="minor", size=0)
x_axis.set_tick_params(which="minor", width=0)
ax1.set_xticks(list(rank_vals))
extra_params |= {r"xtick={}".format(rank_vals)}
# use external package that doesn't round tick labels to 3 sig figs :S
extra_params |= {r"xticklabel={\xinttheiexpr[0]2^\tick\relax}"}

# if Y_LOG:
#     ax1.set_yscale("log")
#     ax1.tick_params(which="both", labelright=True, right=True)
#     # display as 10, 100, 1000 rather than 10^1, 10^2, 10^3, etc
#     extra_params |= {"log ticks with fixed point"}

plt.xlabel("Compute nodes used")
plt.ylabel("Parallel efficiency")
plt.legend()

if EXPORT:
    tikz_save(
        "latex/efficiency_comparison.tex",
        figureheight=r"\figureheight",
        figurewidth=r"\figurewidth",
        extra_axis_parameters=extra_params,
    )

plt.show()
