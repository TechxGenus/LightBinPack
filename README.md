# LightBinPack

LightBinPack is a lightweight library for solving bin packing problems, implementing core algorithms in C++ and providing a Python interface. The following algorithms are currently implemented:

- First-Fit Decreasing (FFD) - Classic implementation
- First-Fit Decreasing Parallel - Parallel optimized implementation
- Next-Fit (NF) - Simple and fast implementation
- Best-Fit Decreasing (BFD) - Best-Fit Decreasing implementation

## Installation

```bash
pip install lightbinpack
```

## Usage

```python
from lightbinpack import ffd, ffd_parallel, nf, bfd

items = [2.5, 1.5, 3.0, 2.0, 1.0]
bin_capacity = 4.0

result_ffd = ffd(items, bin_capacity)

result_ffd_parallel = ffd_parallel(items, bin_capacity, num_threads=4)

result_nf = nf(items, bin_capacity)

result_bfd = bfd(items, bin_capacity)

print(result_ffd)
print(result_ffd_parallel)
print(result_nf)
print(result_bfd)
```

## Algorithm Description

### First-Fit Decreasing (FFD)
1. Sort all items in descending order of size
2. For each item, put it in the first box that can hold it
3. If there is no suitable box, create a new box

### First-Fit Decreasing Parallel
- Parallel optimized version based on FFD algorithm
- Use OpenMP to implement multi-threaded processing
- Suitable for large-scale datasets

### Next-Fit (NF)
- The simplest online packing algorithm
- Only maintain the current box
- If the current item cannot be placed, open a new box

### Best-Fit Decreasing (BFD)
- Sort all items in descending order of size
- For each item, put it in the box with the smallest remaining capacity that can hold it
- If there is no suitable box, create a new box

## Requirements

- Python >= 3.6
- C++ compiler supporting C++11 or higher standard
- pybind11 >= 2.6.0
- OpenMP support (for parallel algorithms)

## Contribution

Contributions are welcome! If you find any bugs or have suggestions for improvements, please open an issue or submit a pull request.
