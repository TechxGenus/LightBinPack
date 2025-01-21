# Heterogeneous Packing

Sometimes due to limited resources, we need to train models in parallel on different devices (for example, on RTX 4090 and RTX 3090). Since different devices have varying computational capabilities, the corresponding packing algorithm needs to be redesigned.

First, we need to calculate the workload for each node based on their MFU (Model FLOPs Utilization) and TPS (Tokens Per Second) under different maximum sequence lengths, resulting in a list of maximum batch lengths. For load balancing, we use a method similar to OGBFD (Optimized Grouped Best-Fit Decreasing). For each bin group, when inserting a new item, we find a suitable bin that ensures the workload remains as balanced as possible across the bin group after insertion.

```python
from lightbinpack import ohgbfd

items = [2, 1, 3, 2, 1]
batch_max_lengths=[4, 5]

result_ohgbfd = ohgbfd(items, batch_max_lengths)

print("pack with strategy ohgbfd:", result_ohgbfd)
```

Using this method, the computational intensity is generally balanced across devices, resulting in minimal bubble time during distributed training. However, memory usage becomes unbalanced, with higher-performance devices using more memory. When memory is constrained, this may prevent further speed improvements.

When using ZeRO-3 or FSDP for data parallel training, we can modify the sharding scheme to distribute tensors unevenly across different devices, further balancing memory usage.
