#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <algorithm>
#include <queue>
#include <stdexcept>

namespace py = pybind11;

class IterativeSegmentTree {
private:
    int n;
    std::vector<int> tree;

public:
    IterativeSegmentTree(int max_length) {
        n = 1;
        while (n < max_length + 1) n <<= 1;
        tree.assign(2 * n, 0);
        tree[n - 1 + max_length] = max_length;
        for (int i = max_length - 1; i >= 0; --i) {
            tree[n - 1 + i] = 0;
        }
        for (int i = n - 2; i >= 0; --i) {
            tree[i] = std::max(tree[2 * i + 1], tree[2 * i + 2]);
        }
    }

    void update(int idx, int val) {
        idx += n - 1;
        tree[idx] = val;
        while (idx > 0) {
            idx = (idx - 1) / 2;
            int left = tree[2 * idx + 1];
            int right = tree[2 * idx + 2];
            int new_val = std::max(left, right);
            if (tree[idx] == new_val) break;
            tree[idx] = new_val;
        }
    }

    int find_best_fit(int target) const {
        int idx = 0;
        if (tree[idx] < target) return -1;
        while (idx < (n - 1)) {
            if (tree[2 * idx + 1] >= target)
                idx = 2 * idx + 1;
            else
                idx = 2 * idx + 2;
        }
        int capacity = idx - (n - 1);
        return tree[idx] >= target ? capacity : -1;
    }
};

class BinGroup {
private:
    int num_bins;
    int batch_max_length;
    std::vector<std::vector<int>> bins;
    std::vector<int> remaining_space;
    std::priority_queue<std::pair<int,int>> max_heap;

public:
    BinGroup(int m, int l) : num_bins(m), batch_max_length(l) {
        bins.resize(m);
        remaining_space.assign(m, l);
        for (int i = 0; i < m; ++i) {
            max_heap.push({remaining_space[i], i});
        }
    }

    bool can_fit(int size) const {
        if (max_heap.empty()) return false;
        return max_heap.top().first >= size;
    }

    int get_max_remaining() const {
        return max_heap.empty() ? 0 : max_heap.top().first;
    }

    void add_item(int item_idx, int size) {
        if (max_heap.empty()) {
            throw std::runtime_error("No bins available in the group");
        }

        auto [space, bin_idx] = max_heap.top();
        max_heap.pop();

        bins[bin_idx].push_back(item_idx);
        remaining_space[bin_idx] -= size;
        
        max_heap.push({remaining_space[bin_idx], bin_idx});
    }

    const std::vector<std::vector<int>>& get_bins() const {
        return bins;
    }
};

std::vector<std::vector<std::vector<int>>> ogbfd(
    const std::vector<int>& lengths,
    int batch_max_length,
    int bins_per_group,
    int item_max_length = -1,
    int strategy = 0
) {
    if (lengths.empty() || batch_max_length <= 0 || bins_per_group <= 0) {
        return {};
    }

    if (item_max_length <= 0) {
        item_max_length = 0;
        for (int length : lengths) {
            item_max_length = std::max(item_max_length, length);
        }
    }

    std::vector<std::vector<int>> count(item_max_length + 1);
    count.reserve(item_max_length + 1);
    for (size_t i = 0; i < lengths.size(); ++i) {
        int len = lengths[i];
        if (len > batch_max_length) {
            throw std::runtime_error("Item size exceeds batch max length");
        }
        if (len > item_max_length) {
            throw std::runtime_error("Item size exceeds item max length");
        }
        if (len <= 0) {
            throw std::runtime_error("Item size must be positive");
        }
        count[len].push_back(i);
    }

    if (strategy == 0) {
        IterativeSegmentTree seg_tree(batch_max_length);

        std::vector<std::vector<size_t>> capacity_to_groups(batch_max_length + 1);
        std::vector<BinGroup> groups;
        groups.reserve(lengths.size() / (2 * bins_per_group) + 1);

        groups.emplace_back(bins_per_group, batch_max_length);
        capacity_to_groups[batch_max_length].push_back(0);
        seg_tree.update(batch_max_length, batch_max_length);

        std::vector<std::vector<std::vector<int>>> result;
        result.reserve(lengths.size() / (2 * bins_per_group) + 1);

        for (int size = item_max_length; size >= 1; --size) {
            for (int orig_idx : count[size]) {
                int best_capacity = seg_tree.find_best_fit(size);

                if (best_capacity != -1) {
                    size_t group_idx = capacity_to_groups[best_capacity].back();
                    capacity_to_groups[best_capacity].pop_back();
                    if (capacity_to_groups[best_capacity].empty()) {
                        seg_tree.update(best_capacity, 0);
                    }

                    groups[group_idx].add_item(orig_idx, size);
                    int new_capacity = groups[group_idx].get_max_remaining();

                    capacity_to_groups[new_capacity].push_back(group_idx);
                    if (new_capacity > 0) {
                        seg_tree.update(new_capacity, new_capacity);
                    }
                } else {
                    size_t new_group_idx = groups.size();
                    groups.emplace_back(bins_per_group, batch_max_length);
                    groups.back().add_item(orig_idx, size);

                    int new_capacity = groups.back().get_max_remaining();
                    capacity_to_groups[new_capacity].push_back(new_group_idx);
                    seg_tree.update(new_capacity, new_capacity);
                }
            }
        }

        result.reserve(groups.size());
        for (const auto& group : groups) {
            result.push_back(group.get_bins());
        }

        if (result.size() >= 2) {
            auto& last_group = result.back();
            const auto& first_group = result.front();
            
            int replacement_idx = 0;
            for (size_t i = 0; i < last_group.size(); ++i) {
                if (last_group[i].empty()) {
                    if (replacement_idx < first_group.size()) {
                        last_group[i] = first_group[replacement_idx];
                        replacement_idx++;
                    }
                }
            }
        }

        return result;
    } else {
        IterativeSegmentTree seg_tree(batch_max_length);
        
        std::vector<std::vector<size_t>> capacity_to_bins(batch_max_length + 1);
        std::vector<size_t> bins_remaining;
        bins_remaining.reserve(lengths.size() / 2);        

        bins_remaining.push_back(batch_max_length);
        capacity_to_bins[batch_max_length].push_back(0);
        seg_tree.update(batch_max_length, batch_max_length);

        std::vector<std::vector<int>> bins_items;
        bins_items.emplace_back();
        bins_items[0].reserve(lengths.size() / 2);

        for (int size = item_max_length; size >= 1; --size) {
            for (int orig_idx : count[size]) {
                int best_capacity = seg_tree.find_best_fit(size);

                if (best_capacity != -1) {
                    size_t bin_idx = capacity_to_bins[best_capacity].back();
                    capacity_to_bins[best_capacity].pop_back();
                    if (capacity_to_bins[best_capacity].empty()) {
                        seg_tree.update(best_capacity, 0);
                    }

                    int new_capacity = bins_remaining[bin_idx] - size;
                    bins_remaining[bin_idx] = new_capacity;

                    bins_items[bin_idx].push_back(orig_idx);

                    capacity_to_bins[new_capacity].push_back(bin_idx);
                    if (new_capacity > 0) {
                        seg_tree.update(new_capacity, new_capacity);
                    }
                } else {
                    size_t new_bin_idx = bins_remaining.size();
                    bins_remaining.push_back(batch_max_length - size);
                    bins_items.emplace_back();
                    bins_items.back().push_back(orig_idx);

                    int new_capacity = batch_max_length - size;
                    capacity_to_bins[new_capacity].push_back(new_bin_idx);
                    seg_tree.update(new_capacity, new_capacity);
                }
            }
        }

        std::vector<std::vector<std::vector<int>>> result;
        for (size_t i = 0; i < bins_items.size(); i += bins_per_group) {
            std::vector<std::vector<int>> group;
            for (size_t j = 0; j < bins_per_group; j++) {
                if (i + j < bins_items.size()) {
                    group.push_back(bins_items[i + j]);
                } else {
                    group.emplace_back();
                }
            }
            result.push_back(group);
        }

        if (result.size() >= 2) {
            auto& last_group = result.back();
            const auto& first_group = result.front();
            
            int replacement_idx = 0;
            for (size_t i = 0; i < last_group.size(); ++i) {
                if (last_group[i].empty() && replacement_idx < first_group.size()) {
                    last_group[i] = first_group[replacement_idx++];
                }
            }
        }

        return result;
    }
}

PYBIND11_MODULE(ogbfd, m) {
    m.doc() = "Optimized Grouped BFD (Best Fit Decreasing) algorithm implementation";
    m.def("ogbfd", &ogbfd, "Optimized Grouped BFD algorithm",
          py::arg("lengths"),
          py::arg("batch_max_length"),
          py::arg("bins_per_group") = 1,
          py::arg("item_max_length") = -1,
          py::arg("strategy") = 0);
}