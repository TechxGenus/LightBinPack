from lightbinpack import radix_sort, radix_merge, load_balance

# Radix Sort
data = [[[4, 0], [7, 8], [4, 11]], [[4, 3], [9, 10]], [[1, 2], [5, 6]]]
sorted_data = radix_sort(data, start_index=0, max_index=9, max_value=40)
print("Radix Sort:", sorted_data)

# Radix Merge
data = [[[1, 2], [5, 6]], [[4, 0], [7, 8], [4, 11]], [[4, 3], [9, 10]]]
merged_data = radix_merge(
    data, min_prefix_match=1, max_length=10, max_count=5, allow_cross_group_merge=False
)
print("Radix Merge:", merged_data)

# Load Balance (experimental)
input_data = [7, 7, 2]
nodes = 2
results = load_balance(input_data, nodes)
print("Load Balance:", results)
