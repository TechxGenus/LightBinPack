# Packing

Packing has been widely used in LLM training to improve training efficiency. It was initially used in pre-training and has recently gained extensive support in post-training as well.

However, the packing approaches may differ between post-training and pre-training stages, and different libraries implement packing differently. The main differences are as follows:

- Whether to allow sequence truncation

If sequence truncation is allowed, packing becomes extremely simple: we only need to pack sequences up to the maximum length and truncate the excess.
However, for instruction data, truncating sequences inevitably leads to information loss, which may affect post-training performance. Therefore, post-training packing typically doesn't allow sequence truncation. Traditional pre-training packing usually allows truncation, though recent literature suggests that sequence truncation may also lead to decreased pre-training performance. In LightBinPack's implementation, neither post-training nor pre-training packing allows sequence truncation.

- Whether to use document masking

Document masking refers to whether packed sequences only consider tokens from the current document when computing attention scores. It needs to be implemented with efficient operators (such as FlashAttention or FlexAttention). In most current implementations, document masking is not used during pre-training but is used during post-training. LightBinPack's design follows this convention.

- Whether to allow sample sorting during packing

Sorted batching is an optimization technique used during training that groups samples of similar lengths to minimize padding. However, this results in samples within the same batch having similar lengths, and the impact on performance is not yet clear. Efficient packing algorithms like ffd (First-Fit Decreasing) and bfd (Best-Fit Decreasing) involve sample sorting and face similar issues.
Some existing implementations believe that sorting operations should not be allowed, thus using the simplest nf (Next-Fit) algorithm for packing or treating it as a multiprocessor scheduling problem. Others argue that this step is necessary for improving training efficiency and doesn't lead to performance degradation, and any impact on training data distribution can be mitigated through gradient accumulation. LightBinPack agrees with the latter view, so its default API allows sample sorting, using bfd as the base algorithm with specific modifications.

## Pre-training Stage

The pre-training stage uses the obfd (Optimized Best-Fit Decreasing) algorithm, which optimizes bfd by using counting sort instead of comparison sort and employs a segment tree for efficient bin search.

```python
from lightbinpack import pack

lengths = [20, 20, 10, 10, 10, 10]
batch_max_length = 40

# For linear attention or disabled document mask
results = pack(lengths, batch_max_length, variant="linear")
print("pack with variant linear:", results)
```

## Post-training Stage

Due to document masking, there might be load imbalance issues between `n` data parallel nodes during post-training. Therefore, the post-training stage uses the ogbfd (Optimized Grouped Best-Fit Decreasing) algorithm, which modifies obfd using two different strategies: one directly groups based on the default execution results, since the bfd algorithm sorts first, the samples in roughly sorted bins have similar lengths. The other strategy is to treat every `n` bins as a bin group during obfd execution and process them together, using the wfd (Worst-Fit Decreasing) algorithm within bin groups (or it can be viewed as a multiprocessor scheduling problem) to balance the load between data parallel nodes.

```python
from lightbinpack import pack

lengths = [20, 20, 10, 10, 10, 10]
batch_max_length = 40

# For attention with document mask enabled
results = pack(lengths, batch_max_length, variant="square", dp_size=2)
print("pack with variant square:", results)
```
