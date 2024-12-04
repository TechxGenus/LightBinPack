#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <algorithm>
#include <omp.h>
#include <mutex>
#include <stdexcept>

namespace py = pybind11;

class SegmentTree {
public:
    SegmentTree(size_t size) : size(size) {
        tree.resize(4 * size, 0.0);
    }

    void build(const std::vector<double>& bins_remaining_space) {
        build(1, 0, size - 1, bins_remaining_space);
    }

    int query(double size_needed) {
        return query(1, 0, this->size - 1, size_needed);
    }

    void update(int idx, double value) {
        update(1, 0, size - 1, idx, value);
    }

private:
    size_t size;
    std::vector<double> tree;

    void build(int node, int start, int end, const std::vector<double>& bins_remaining_space) {
        if (start > end) return;
        if (start == end) {
            if (start < bins_remaining_space.size()) {
                tree[node] = bins_remaining_space[start];
            }
        } else {
            int mid = (start + end) / 2;
            build(2 * node, start, mid, bins_remaining_space);
            build(2 * node + 1, mid + 1, end, bins_remaining_space);
            tree[node] = std::max(tree[2 * node], tree[2 * node + 1]);
        }
    }

    int query(int node, int start, int end, double size_needed) {
        if (tree[node] < size_needed) {
            return -1;
        }
        if (start == end) {
            return start;
        }
        int mid = (start + end) / 2;
        int left_result = query(2 * node, start, mid, size_needed);
        if (left_result != -1) {
            return left_result;
        }
        return query(2 * node + 1, mid + 1, end, size_needed);
    }

    void update(int node, int start, int end, int idx, double value) {
        if (start > end || idx < start || idx > end) {
            return;
        }
        if (start == end) {
            tree[node] = value;
        } else {
            int mid = (start + end) / 2;
            update(2 * node, start, mid, idx, value);
            update(2 * node + 1, mid + 1, end, idx, value);
            tree[node] = std::max(tree[2 * node], tree[2 * node + 1]);
        }
    }
};

class Bin {
public:
    double remaining_space;
    size_t bin_index;
    std::vector<int> items;

    Bin(double space, size_t index) : 
        remaining_space(space), 
        bin_index(index) {}
};

std::vector<std::vector<int>> ffd_parallel(const std::vector<double>& lengths, double batch_max_length, int num_threads = -1) {
    if (lengths.empty() || batch_max_length <= 0) {
        return {};
    }

    if (num_threads > 0) {
        omp_set_num_threads(num_threads);
    }

    std::vector<std::pair<double, int>> length_pairs;
    length_pairs.reserve(lengths.size());
    for(size_t i = 0; i < lengths.size(); i++) {
        if (lengths[i] > batch_max_length) {
            throw std::runtime_error("Item size exceeds batch max length");
        }
        length_pairs.emplace_back(lengths[i], i);
    }
    
    std::sort(length_pairs.begin(), length_pairs.end(), 
              std::greater<std::pair<double, int>>());

    const size_t total_items = length_pairs.size();
    const size_t chunk_size = std::max(size_t(1000), total_items / (omp_get_max_threads() * 4));
    std::vector<std::vector<std::vector<int>>> thread_results(omp_get_max_threads());

    #pragma omp parallel
    {
        const int thread_id = omp_get_thread_num();
        std::vector<Bin> local_bins;
        std::vector<double> bins_remaining_space;
        size_t max_bins = total_items;
        bins_remaining_space.reserve(max_bins);

        SegmentTree segment_tree(max_bins);

        #pragma omp for schedule(dynamic, chunk_size)
        for(size_t i = 0; i < total_items; i++) {
            const auto& pair = length_pairs[i];
            double size = pair.first;
            int orig_idx = pair.second;

            int bin_idx = -1;
            if (!bins_remaining_space.empty()) {
                bin_idx = segment_tree.query(size);
            }

            if (bin_idx != -1 && bin_idx < local_bins.size()) {
                Bin& bin = local_bins[bin_idx];
                bin.remaining_space -= size;
                bin.items.push_back(orig_idx);
                bins_remaining_space[bin_idx] = bin.remaining_space;
                segment_tree.update(bin_idx, bins_remaining_space[bin_idx]);
            } else {
                Bin new_bin(batch_max_length - size, local_bins.size());
                new_bin.items.push_back(orig_idx);
                local_bins.push_back(new_bin);
                bins_remaining_space.push_back(new_bin.remaining_space);
                segment_tree.update(local_bins.size() - 1, new_bin.remaining_space);
            }
        }

        thread_results[thread_id].resize(local_bins.size());
        for (size_t idx = 0; idx < local_bins.size(); ++idx) {
            thread_results[thread_id][idx] = local_bins[idx].items;
        }
    }

    std::vector<std::vector<int>> final_result;
    for (const auto& thread_result : thread_results) {
        final_result.insert(final_result.end(), 
                            thread_result.begin(), 
                            thread_result.end());
    }

    return final_result;
}

PYBIND11_MODULE(ffd_parallel, m) {
    m.doc() = "Parallel FFD algorithm implementation in C++";
    m.def("ffd_parallel", &ffd_parallel, 
          "Parallel FFD algorithm",
          py::arg("lengths"),
          py::arg("batch_max_length"),
          py::arg("num_threads") = -1);
}
