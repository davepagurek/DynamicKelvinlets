#!/usr/bin/env python3

from matplotlib import pyplot as plt
import glob
import re
import math

def load_obj_vertices(path):
    vertices = []
    pattern = re.compile(r"^v (-?[\d\.]+) (-?[\d\.]+) (-?[\d\.]+)$")
    with open(path) as fp:
        for line in fp:
            m = pattern.match(line.rstrip())
            if m:
                vertices.append((float(m.group(1)), float(m.group(2)), float(m.group(3))))
    return vertices


def calc_energies(pattern):
    last_positions = None
    scale = None
    energies = []
    first = None
    for path in sorted(glob.glob(pattern)):
        next_positions = load_obj_vertices(path)
        m = 1/len(next_positions)
        if not(last_positions is None):
            energy = 0
            for a, b in zip(last_positions, next_positions):
                diff = (b[0]-a[0], b[1]-a[1], b[2]-a[2])
                v_sq = (diff[0]**2 + diff[1]**2 + diff[2]**2)
                energy += 0.5*m*v_sq
            energies.append(energy)
            if first is None:
                first = energy
        last_positions = next_positions

    return [ e / first for e in energies ]

fig, ax = plt.subplots(1, 1, sharex=True, figsize=(7, 3))
fig.suptitle("Simulation Energy Loss")
ax.set_ylabel("Kinetic Energy")
ax.set_xlabel("Time (s)")
for (pattern, name) in [("dk/*.obj", "Dynamic Kelvinlets"), ("bullet/*.obj", "Bullet")]:
    energies = calc_energies(pattern)
    while len(energies) < 136:
        energies.append(0)
    ax.plot([ t/24 for t in range(0, len(energies)) ], energies, label=name)
fig.tight_layout(rect=[0, 0.03, 1, 0.95])
ax.legend()
plt.show()
