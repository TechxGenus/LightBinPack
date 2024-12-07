# LightBinPack

LightBinPack is a lightweight library for solving bin packing problems, implementing core algorithms in C++ and providing a Python interface, and has been optimized for bin packing algorithms in both pre-training and post-training scenarios for LLMs. The following algorithms are currently implemented:

- Next-Fit (NF) - Simple and fast implementation
- First-Fit Decreasing (FFD) - Classic implementation
- Best-Fit Decreasing (BFD) - Best-Fit Decreasing implementation
- Optimized Best-Fit Decreasing (OBFD) - Optimized BFD for integer lengths
- Optimized Best-Fit Decreasing Parallel (OBFDP) - Parallel version of OBFD for large integer lengths
- Optimized Grouped Best-Fit Decreasing (OGBFD) - Group-based BFD for better load balancing
- Optimized Grouped Best-Fit Decreasing Parallel (OGBFDP) - Parallel version of OGBFD for large datasets

## Installation

```bash
pip install lightbinpack
```

If you want to use the latest features, you can install the development version from GitHub:

```bash
pip install git+https://github.com/TechxGenus/LightBinPack.git
```

## Usage

```python
from lightbinpack import nf, ffd, bfd, obfd, obfdp, ogbfd, ogbfdp

items = [2.5, 1.5, 3.0, 2.0, 1.0]
bin_capacity = 4.0

result_nf = nf(items, bin_capacity)
result_ffd = ffd(items, bin_capacity)
result_bfd = bfd(items, bin_capacity)

items_int = [2, 1, 3, 2, 1]
bin_capacity_int = 4

result_obfd = obfd(items_int, bin_capacity_int)
result_obfdp = obfdp(items_int, bin_capacity_int)
result_ogbfd = ogbfd(items_int, bin_capacity_int, bins_per_group=2)
result_ogbfdp = ogbfdp(items_int, bin_capacity_int, bins_per_group=2)

print(result_nf)
print(result_ffd)
print(result_bfd)
print(result_obfd)
print(result_obfdp)
print(result_ogbfd)
print(result_ogbfdp)
```

## Algorithm Description

### Next-Fit (NF)
- The simplest online packing algorithm
- Only maintains the current box
- If the current item cannot be placed, opens a new box
- Linear time complexity: O(N)

### First-Fit Decreasing (FFD)
- Sort all items in descending order of size
- For each item, put it in the first box that can hold it
- If there is no suitable box, create a new box
- Time complexity: O(N log N)

### Best-Fit Decreasing (BFD)
- Sort all items in descending order of size
- For each item, put it in the box with the smallest remaining capacity that can hold it
- If there is no suitable box, create a new box
- Time complexity: O(N log N)

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

### Optimized Grouped Best-Fit Decreasing Parallel (OGBFDP)
- Parallel version of OGBFD for large datasets
- Automatically splits items into multiple chunks for parallel processing
- Uses OpenMP for parallel execution
- Includes a repack phase for better bin utilization
- Maintains group-based bin allocation for load balancing
- Suitable for large datasets requiring balanced bin utilization
- Adaptive to available CPU cores and input size

## Algorithm Selection Guide

For real-time applications with streaming data or limited memory, Next-Fit (NF) is the simplest choice despite using more bins. First-Fit Decreasing (FFD) and Best-Fit Decreasing (BFD) are more complex but offer better bin utilization. When working with integer-length items, such as token lengths, Optimized Best-Fit Decreasing (OBFD) excels in memory and storage optimization scenarios. For large-scale integer datasets, OBFDP leverages parallel processing for improved performance. For the distributed training scenario of LLM with quadratic attention, OGBFD provides both better bin utilization and load balancing, and OGBFDP further accelerates the process with parallel execution.

To determine which algorithm offers the best efficiency and performance for your infrastructure, consider running `bench.py` and `bench_balance.py` to analyze the detailed metrics and results.

## Requirements

- Python >= 3.6
- C++ compiler supporting C++11 or higher standard
- pybind11 >= 2.6.0
- OpenMP support (for parallel algorithms)

## Acknowledgements

- [Multipack Sampler](https://github.com/imoneoi/multipack_sampler)
- [OBFD](https://arxiv.org/abs/2404.10830)
- Claude and GPT(o1)

## Contribution

Contributions are welcome! If you find any bugs or have suggestions for improvements, please open an issue or submit a pull request.
