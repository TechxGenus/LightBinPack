# Algorithms
The following algorithms are currently implemented:

- Next-Fit (NF) - Simple and fast implementation
- First-Fit Decreasing (FFD) - Classic implementation
- Best-Fit Decreasing (BFD) - Best-Fit Decreasing implementation
- Optimized Best-Fit Decreasing (OBFD) - Optimized BFD for integer lengths
- Optimized Best-Fit Decreasing Parallel (OBFDP) - Parallel version of OBFD for large integer lengths
- Optimized Grouped Best-Fit Decreasing (OGBFD) - Group-based BFD for better load balancing
- Optimized Grouped Best-Fit Decreasing Parallel (OGBFDP) - Parallel version of OGBFD for large datasets
- Optimized Heterogeneous Grouped Best-Fit Decreasing (OHGBFD) - Group-based BFD with heterogeneous bin sizes
- Optimized Sequential Heterogeneous Grouped Best-Fit Decreasing (OSHGBFD) - Sequential version of OHGBFD

## Usage

```python
from lightbinpack import pack

lengths = [20, 20, 10, 10, 10, 10]
batch_max_length = 40

# For linear attention or disabled document mask
results = pack(lengths, batch_max_length, variant="linear")
print("pack with variant linear:", results)

# For attention with document mask enabled
results = pack(lengths, batch_max_length, variant="square", dp_size=2)
print("pack with variant square:", results)

# call directly
from lightbinpack import nf, ffd, bfd  # noqa: E402

items = [2.5, 1.5, 3.0, 2.0, 1.0]
bin_capacity = 4.0

results_nf = nf(items, bin_capacity)
results_ffd = ffd(items, bin_capacity)
results_bfd = bfd(items, bin_capacity)

print("pack with strategy nf:", results_nf)
print("pack with strategy ffd:", results_ffd)
print("pack with strategy bfd:", results_bfd)

items = [2, 1, 3, 2, 1]
batch_max_length = 4

from lightbinpack import obfd, obfdp, ogbfd, ogbfdp, ohgbfd, oshgbfd  # noqa: E402

result_obfd = obfd(items, batch_max_length)
result_obfdp = obfdp(items, batch_max_length)
result_ogbfd = ogbfd(items, batch_max_length, bins_per_group=2)
result_ogbfdp = ogbfdp(items, batch_max_length, bins_per_group=2)
result_ohgbfd = ohgbfd(items, batch_max_lengths=[4, 5])
result_oshgbfd = oshgbfd(items, batch_max_lengths_list=[[4, 5], [6]])

print("pack with strategy obfd:", result_obfd)
print("pack with strategy obfdp:", result_obfdp)
print("pack with strategy ogbfd:", result_ogbfd)
print("pack with strategy ogbfdp:", result_ogbfdp)
print("pack with strategy ohgbfd:", result_ohgbfd)
print("pack with strategy oshgbfd:", result_oshgbfd)
```

## Description

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

### Optimized Heterogeneous Grouped Best-Fit Decreasing (OHGBFD)
- Group-based version of BFD supporting different bin sizes within groups
- Uses segment tree for efficient group capacity tracking
- Maintains multiple bins per group with heterogeneous capacities
- Time complexity: O(N log L) where L is the maximum length
- Suitable for scenarios requiring balanced bin utilization with varying bin sizes

### Optimized Sequential Heterogeneous Grouped Best-Fit Decreasing (OSHGBFD)
- Sequential version of OHGBFD, suitable for context parallel training
- Uses segment tree for efficient group capacity tracking
- Maintains multiple bins per group with heterogeneous capacities
- Time complexity: O(N log L) where L is the maximum length
- Suitable for scenarios requiring balanced bin utilization with varying bin sizes

## Algorithm Selection Guide

For real-time applications with streaming data or limited memory, Next-Fit (NF) is the simplest choice despite using more bins. First-Fit Decreasing (FFD) and Best-Fit Decreasing (BFD) are more complex but offer better bin utilization. When working with integer-length items, such as token lengths, Optimized Best-Fit Decreasing (OBFD) excels in memory and storage optimization scenarios. For large-scale integer datasets, OBFDP leverages parallel processing for improved performance. For the distributed training scenario of LLM with quadratic attention, OGBFD provides both better bin utilization and load balancing, and OGBFDP further accelerates the process with parallel execution, while it may slightly reduce packing efficiency and load balancing.

To determine which algorithm offers the best efficiency and performance for your infrastructure, consider running `bench.py` and `bench_balance.py` to analyze the detailed metrics and results.
