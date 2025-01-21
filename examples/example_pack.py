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

from lightbinpack import obfd, obfdp, ogbfd, ogbfdp, ohgbfd, oshgbfd  # noqa: E402

items = [2, 1, 3, 2, 1]
batch_max_length = 4

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
