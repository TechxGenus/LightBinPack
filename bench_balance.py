import time
import numpy as np
import matplotlib.pyplot as plt
from lightbinpack import ogbfd

def verify_packing(original_lengths, bin_results, max_length):
    """Verify if the packing result is valid."""
    packed_items = set()
    for group in bin_results:
        for bin_items in group:
            packed_items.update(bin_items)
    
    if len(packed_items) != len(original_lengths):
        return False
    
    for group in bin_results:
        for bin_items in group:
            if sum(original_lengths[i] for i in bin_items) > max_length:
                return False
    
    return True

def calculate_utilization(original_lengths, bin_results, max_length):
    """Calculate space utilization"""
    total_bins = sum(len(group) for group in bin_results)
    used_space = sum(sum(original_lengths[i] for i in bin_items) 
                    for group in bin_results 
                    for bin_items in group)
    total_space = total_bins * max_length
    return used_space / total_space

def calculate_square_sum_balance(original_lengths, bin_results):
    """Calculate load balance metrics using sum of squares for each group"""
    if not bin_results:
        return 0.0
    
    group_imbalances = []
    for group in bin_results:
        group_square_sums = [
            sum(original_lengths[i] * original_lengths[i] for i in bin_items)
            for bin_items in group
            if bin_items
        ]
        
        if group_square_sums:
            max_square_sum = max(group_square_sums)
            min_square_sum = min(group_square_sums)
            group_imbalance = max_square_sum - min_square_sum
            group_imbalances.append(group_imbalance)
    
    return sum(group_imbalances) / len(group_imbalances) if group_imbalances else 0.0

def count_bin_groups(bin_results):
    """Count the number of bin groups in the packing result"""
    return len(bin_results)

def run_balance_benchmark(algorithm, sizes, lengths, max_length, bins_per_group=1, strategy=0, num_runs=3):
    """Run benchmark test focusing on load balance metrics"""
    time_results = []
    util_results = []
    balance_results = []
    group_count_results = []
    
    for size in sizes:
        total_time = 0
        total_util = 0
        total_balance = 0
        total_groups = 0
        
        for _ in range(num_runs):
            data = lengths[:size]
            
            start = time.time()
            if algorithm.__name__ == 'ogbfd':
                result = algorithm(data, max_length, bins_per_group, strategy=strategy)
            else:
                result = algorithm(data, max_length)
            end = time.time()
            
            assert verify_packing(data, result, max_length), \
                   f"Algorithm {algorithm.__name__} failed validation at data size {size}"
            
            total_time += end - start
            total_util += calculate_utilization(data, result, max_length)
            total_balance += calculate_square_sum_balance(data, result)
            total_groups += count_bin_groups(result)
            
        time_results.append(total_time / num_runs)
        util_results.append(total_util / num_runs)
        balance_results.append(total_balance / num_runs)
        group_count_results.append(total_groups / num_runs)
        
    return time_results, util_results, balance_results, group_count_results

def plot_balance_results(sizes, results_dict):
    """Plot performance comparison charts"""
    _, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 12))
    
    styles = {
        'OGBFD-1-S0': ('b', 'o'),
        'OGBFD-2-S0': ('r', 'o'),
        'OGBFD-4-S0': ('g', 'o'),
        'OGBFD-8-S0': ('y', 'o'),
        'OGBFD-1-S1': ('b', 's'),
        'OGBFD-2-S1': ('r', 's'),
        'OGBFD-4-S1': ('g', 's'),
        'OGBFD-8-S1': ('y', 's')
    }
    
    for name, (times, _, _, _) in results_dict.items():
        color, marker = styles[name]
        ax1.plot(sizes, times, color=color, marker=marker, label=name, linestyle='-')
    
    ax1.set_xlabel('Input Size')
    ax1.set_ylabel('Average Time (s)')
    ax1.set_title('Time Performance')
    ax1.grid(True)
    ax1.legend()
    
    for name, (_, utils, _, _) in results_dict.items():
        color, marker = styles[name]
        ax2.plot(sizes, utils, color=color, marker=marker, label=name, linestyle='-')
    
    ax2.set_xlabel('Input Size')
    ax2.set_ylabel('Space Utilization')
    ax2.set_title('Space Utilization')
    ax2.grid(True)
    ax2.legend()
    ax2.yaxis.set_major_formatter(plt.FuncFormatter(lambda y, _: '{:.3%}'.format(y)))
    
    for name, (_, _, balance, _) in results_dict.items():
        color, marker = styles[name]
        ax3.plot(sizes, balance, color=color, marker=marker, label=name, linestyle='-')
    
    ax3.set_xlabel('Input Size')
    ax3.set_ylabel('Square Sum Imbalance')
    ax3.set_title('Load Balance')
    ax3.grid(True)
    ax3.legend()
    
    for name, (_, _, _, groups) in results_dict.items():
        color, marker = styles[name]
        ax4.plot(sizes, groups, color=color, marker=marker, label=name, linestyle='-')
    
    ax4.set_xlabel('Input Size')
    ax4.set_ylabel('Number of Bin Groups')
    ax4.set_title('Bin Groups Count')
    ax4.grid(True)
    ax4.legend()
    
    plt.tight_layout()
    plt.savefig('tmp/balance_benchmark_results.png', bbox_inches='tight')
    plt.close()

def main():
    sizes = [10000, 20000, 50000, 100000, 200000, 500000, 1000000]
    batch_max_length = 50000
    num_runs = 5
    
    np.random.seed(0)
    max_data_size = max(sizes)
    lengths = np.random.randint(1000, 20000, max_data_size)
    
    results = {
        'OGBFD-1-S0': run_balance_benchmark(ogbfd, sizes, lengths, batch_max_length, 1, strategy=0, num_runs=num_runs),
        'OGBFD-2-S0': run_balance_benchmark(ogbfd, sizes, lengths, batch_max_length, 2, strategy=0, num_runs=num_runs),
        'OGBFD-4-S0': run_balance_benchmark(ogbfd, sizes, lengths, batch_max_length, 4, strategy=0, num_runs=num_runs),
        'OGBFD-8-S0': run_balance_benchmark(ogbfd, sizes, lengths, batch_max_length, 8, strategy=0, num_runs=num_runs),
        'OGBFD-1-S1': run_balance_benchmark(ogbfd, sizes, lengths, batch_max_length, 1, strategy=1, num_runs=num_runs),
        'OGBFD-2-S1': run_balance_benchmark(ogbfd, sizes, lengths, batch_max_length, 2, strategy=1, num_runs=num_runs),
        'OGBFD-4-S1': run_balance_benchmark(ogbfd, sizes, lengths, batch_max_length, 4, strategy=1, num_runs=num_runs),
        'OGBFD-8-S1': run_balance_benchmark(ogbfd, sizes, lengths, batch_max_length, 8, strategy=1, num_runs=num_runs)
    }
    
    print("\nBalance Benchmark Results:")
    print("-" * 62)
    print(f"{'Size':>8} {'Algorithm':>12} {'Time(s)':>8} {'Util%':>7} {'Imbalance':>12} {'Groups':>7}")
    print("-" * 62)
    
    for size_idx, size in enumerate(sizes):
        for algo in results:
            time_taken = results[algo][0][size_idx]
            utilization = results[algo][1][size_idx]
            balance = results[algo][2][size_idx]
            groups = results[algo][3][size_idx]
            print(f"{size:>8} {algo:>12} {time_taken:>8.3f} {utilization:>7.1%} "
                  f"{balance:>12.0f} {groups:>7.0f}")
    
    plot_balance_results(sizes, results)
    print("\nThe balance benchmark results have been saved as 'balance_benchmark_results.png'")

if __name__ == "__main__":
    main()
