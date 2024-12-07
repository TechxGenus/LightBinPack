# LightBinPack

LightBinPack is a lightweight library for solving bin packing problems, implementing core algorithms in C++ and providing a Python interface. The following algorithms are currently implemented:

- First-Fit Decreasing (FFD) - Classic implementation
- Next-Fit (NF) - Simple and fast implementation
- Best-Fit Decreasing (BFD) - Best-Fit Decreasing implementation
- Optimized Best-Fit Decreasing (OBFD) - Optimized BFD for integer lengths
- Optimized Best-Fit Decreasing Parallel (OBFDP) - Parallel version of OBFD for large integer lengths
- Optimized Grouped Best-Fit Decreasing (OGBFD) - Group-based BFD for better load balancing

## Installation

```bash
pip install lightbinpack
```

## Usage

```python
from lightbinpack import ffd, nf, bfd, obfd, obfdp, ogbfd

items = [2.5, 1.5, 3.0, 2.0, 1.0]
bin_capacity = 4.0

result_ffd = ffd(items, bin_capacity)
result_nf = nf(items, bin_capacity)
result_bfd = bfd(items, bin_capacity)

items_int = [2, 1, 3, 2, 1]
bin_capacity_int = 4

result_obfd = obfd(items_int, bin_capacity_int)
result_obfdp = obfdp(items_int, bin_capacity_int)
result_ogbfd = ogbfd(items_int, bin_capacity_int, bins_per_group=2)

print(result_ffd)
print(result_nf)
print(result_bfd)
print(result_obfd)
print(result_obfdp)
print(result_ogbfd)
```

## Algorithm Description

### First-Fit Decreasing (FFD)
1. Sort all items in descending order of size
2. For each item, put it in the first box that can hold it
3. If there is no suitable box, create a new box

### Next-Fit (NF)
- The simplest online packing algorithm
- Only maintain the current box
- If the current item cannot be placed, open a new box

### Best-Fit Decreasing (BFD)
- Sort all items in descending order of size
- For each item, put it in the box with the smallest remaining capacity that can hold it
- If there is no suitable box, create a new box

### Optimized Best-Fit Decreasing (OBFD)
- Optimized version of BFD for integer lengths
- Uses counting sort instead of comparison sort
- Employs segment tree for efficient bin search
- Time complexity: O(N log L) where L is the maximum length
- Suitable for cases where item lengths are integers and L << N

### Optimized Best-Fit Decreasing Parallel (OBFDP)
- Parallel version of OBFD for large integer lengths
- Automatically splits items into multiple chunks for parallel processing
- Uses OpenMP for parallel execution
- Includes a repack phase for better bin utilization
- Suitable for large datasets with integer lengths
- Adaptive to available CPU cores and input size

### Optimized Grouped Best-Fit Decreasing (OGBFD)
- Group-based version of BFD for better load balancing
- Uses segment tree for efficient group capacity tracking
- Maintains multiple bins per group for balanced distribution
- Time complexity: O(N log L) where L is the maximum length
- Suitable for scenarios requiring balanced bin utilization

## Requirements

- Python >= 3.6
- C++ compiler supporting C++11 or higher standard
- pybind11 >= 2.6.0
- OpenMP support (for parallel algorithms)

## Contribution

Contributions are welcome! If you find any bugs or have suggestions for improvements, please open an issue or submit a pull request.
