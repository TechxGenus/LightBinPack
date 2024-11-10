# LightBinPack

LightBinPack is a lightweight library for solving bin packing problems, implementing core algorithms in C++ and providing a Python interface. Currently, the library implements the classic First-Fit Decreasing (FFD) algorithm.

## Installation

```bash
pip install lightbinpack
```

## Usage

```python
from lightbinpack import ffd

items = [2.5, 1.5, 3.0, 2.0, 1.0]
bin_capacity = 4.0

result = ffd(items, bin_capacity)
print(result)
```

## Algorithm Explanation

First-Fit Decreasing (FFD) algorithm steps:
1. Sort all items in descending order of size
2. For each item, place it in the first container that can accommodate it
3. If no suitable container is found, create a new container

## Requirements

- Python >= 3.6
- C++ compiler supporting C++11 or higher standard
- pybind11 >= 2.6.0

## Contribution

Contributions are welcome! If you find any bugs or have suggestions for improvements, please open an issue or submit a pull request.
