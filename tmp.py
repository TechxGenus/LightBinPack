from lightbinpack import load_balance
import itertools

# lengths 初始值
lengths = [9, 9, 2]
# 节点数量
nodes = 2

# 计算总负载和累积负载
sum_lengths = sum(lengths)
cumsum_lengths = [0] + list(itertools.accumulate(lengths))

# 计算某个排列下的负载差异
def calculation(cumsum_lengths, sum_lengths, lengths, node_idx, nodes):
    start_idx = 0
    for i, val in enumerate(cumsum_lengths):
        if val > node_idx * (sum_lengths / (2 * nodes)):
            start_idx = i - 1 if i > 0 else 0
            break
    
    end_idx = 0
    for i, val in enumerate(cumsum_lengths):
        if val >= (node_idx + 1) * (sum_lengths / (2 * nodes)):
            end_idx = i - 1 if i > 0 else 0
            break
    
    c = 0
    for i in range(start_idx, end_idx):
        c += lengths[i] ** 2 / 2
    
    c -= (node_idx * (sum_lengths / (2 * nodes)) - cumsum_lengths[start_idx]) ** 2 / 2
    c += ((node_idx + 1) * (sum_lengths / (2 * nodes)) - cumsum_lengths[end_idx]) ** 2 / 2
    return c

# 计算负载均衡
def get_balance(lengths, nodes):
    sum_lengths = sum(lengths)
    cumsum_lengths = [0] + list(itertools.accumulate(lengths))

    # 计算每个节点的负载均衡
    calculations = [
        calculation(cumsum_lengths, sum_lengths, lengths, node_idx, nodes) +
        calculation(cumsum_lengths, sum_lengths, lengths, 2 * nodes - node_idx - 1, nodes)
        for node_idx in range(nodes)
    ]
    balance = max(abs(c - sum(calculations) / len(calculations)) for c in calculations)
    print(lengths, balance)
    return balance

# 尝试所有排列，找到负载最均衡的排列
best_balance = float('inf')
best_lengths = None
for perm in itertools.permutations(lengths):
    balance = get_balance(list(perm), nodes)
    if balance < best_balance:
        best_balance = balance
        best_lengths = perm

print("最佳排列:", best_lengths)
print("负载均衡度:", best_balance)

print(load_balance([[9, 9, 2], [6, 6, 5], [18, 18, 18, 2]], nodes))