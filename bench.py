import time
import numpy as np
import matplotlib.pyplot as plt
from lightbinpack import ffd, ffd_parallel, nf

def verify_packing(original_lengths, bin_results, max_length):
    """Verify if the packing result is valid."""
    packed_items = set()
    for bin_items in bin_results:
        packed_items.update(bin_items)
    if len(packed_items) != len(original_lengths):
        return False
    
    for bin_items in bin_results:
        if sum(original_lengths[i] for i in bin_items) > max_length:
            return False
    
    return True

def calculate_utilization(original_lengths, bin_results, max_length):
    """Calculate space utilization"""
    used_space = sum(sum(original_lengths[i] for i in bin_items) for bin_items in bin_results)
    total_space = len(bin_results) * max_length
    return used_space / total_space

def run_benchmark(algorithm, sizes, lengths, max_length, num_runs=3):
    """Run benchmark test"""
    time_results = []
    util_results = []
    for size in sizes:
        total_time = 0
        total_util = 0
        for _ in range(num_runs):
            data = lengths[:size]
            start = time.time()
            if algorithm.__name__ == 'ffd_parallel':
                result = algorithm(data, max_length)
            else:
                result = algorithm(data, max_length)
            end = time.time()
            
            assert verify_packing(data, result, max_length), \
                   f"Algorithm {algorithm.__name__} failed validation at data size {size}"
            
            total_time += end - start
            total_util += calculate_utilization(data, result, max_length)
            
        time_results.append(total_time / num_runs)
        util_results.append(total_util / num_runs)
    return time_results, util_results

def plot_results(sizes, results_dict):
    """Plot performance comparison chart, including time and utilization sub-charts"""
    
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
    
    styles = {
        'FFD': ('b', 'o'),
        'FFD Parallel': ('y', 'o'),
        'NF': ('r', 'o')
    }
    
    for name, (times, _) in results_dict.items():
        color, marker = styles[name]
        ax1.plot(sizes, times, color=color, marker=marker, label=name, linestyle='-')
    
    ax1.set_xlabel('Input Size')
    ax1.set_ylabel('Average Time (s)')
    ax1.set_title('Time Performance Comparison')
    ax1.grid(True)
    ax1.legend(bbox_to_anchor=(1.02, 1), loc='upper left')
    
    min_time = min(min(times) for times, _ in results_dict.values())
    max_time = max(max(times) for times, _ in results_dict.values())
    y_margin = (max_time - min_time) * 0.1
    ax1.set_ylim(max(0, min_time - y_margin), max_time + y_margin)
    
    for name, (_, utils) in results_dict.items():
        color, marker = styles[name]
        ax2.plot(sizes, utils, color=color, marker=marker, label=name, linestyle='-')
    
    ax2.set_xlabel('Input Size')
    ax2.set_ylabel('Space Utilization')
    ax2.set_title('Space Utilization Comparison')
    ax2.grid(True)
    ax2.legend(bbox_to_anchor=(1.02, 1), loc='upper left')
    
    min_util = min(min(utils) for _, utils in results_dict.values())
    max_util = max(max(utils) for _, utils in results_dict.values())
    util_margin = (max_util - min_util) * 0.1
    ax2.set_ylim(min_util - util_margin, max_util + util_margin)
    ax2.yaxis.set_major_formatter(plt.FuncFormatter(lambda y, _: '{:.1%}'.format(y)))
    
    plt.tight_layout()
    plt.subplots_adjust(right=0.9)
    
    plt.savefig('benchmark_results.png', bbox_inches='tight')
    plt.close()

def main():
    sizes = [10000, 20000, 50000, 100000, 200000, 500000, 1000000, 2000000, 5000000, 10000000]
    max_length = 50000
    num_runs = 5
    
    np.random.seed(0)
    max_data_size = max(sizes)
    lengths = np.random.randint(1000, 20000, max_data_size)
    
    results = {
        'FFD': run_benchmark(ffd, sizes, lengths, max_length, num_runs),
        'FFD Parallel': run_benchmark(
            lambda x, y: ffd_parallel(x, y),
            sizes, lengths, max_length, num_runs
        ),
        'NF': run_benchmark(nf, sizes, lengths, max_length, num_runs)
    }
    
    print("\nBenchmark Test Results:")
    print("-" * 120)
    print(f"{'Input Size':>12} {'FFD (s)':>9} {'FFD Utilization':>17} {'FFD Parallel (s)':>18} {'FFD Parallel Utilization':>26} "
          f"{'NF (s)':>8} {'NF Utilization':>16}")
    print("-" * 120)
    
    for i, size in enumerate(sizes):
        print(f"{size:>12} "
              f"{results['FFD'][0][i]:>9.3f} {results['FFD'][1][i]:>17.1%} "
              f"{results['FFD Parallel'][0][i]:>18.3f} {results['FFD Parallel'][1][i]:>26.1%} "
              f"{results['NF'][0][i]:>8.3f} {results['NF'][1][i]:>16.1%}")
    
    plot_results(sizes, results)
    print("\nThe result chart has been saved as 'benchmark_results.png'")

if __name__ == "__main__":
    main()