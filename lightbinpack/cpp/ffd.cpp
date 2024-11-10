#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <set>
#include <algorithm>
#include <memory>

namespace py = pybind11;

class Bin {
public:
    double remaining_space;
    size_t bin_index;
    std::vector<int> items;

    Bin(double space, size_t index) : 
        remaining_space(space), 
        bin_index(index) {}

    bool operator<(const Bin& other) const {
        if (remaining_space == other.remaining_space)
            return bin_index < other.bin_index;
        return remaining_space < other.remaining_space;
    }
};

std::vector<std::vector<int>> ffd(const std::vector<double>& lengths, double batch_max_length) {
    if (lengths.empty() || batch_max_length <= 0) {
        return {};
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

    std::vector<std::vector<int>> bins_result;
    std::set<Bin> available_bins;

    for(const auto& pair : length_pairs) {
        double size = pair.first;
        int orig_idx = pair.second;

        auto it = available_bins.lower_bound(Bin(size, 0));
        
        if (it != available_bins.end()) {
            Bin current_bin = *it;
            available_bins.erase(it);
            
            current_bin.remaining_space -= size;
            current_bin.items.push_back(orig_idx);
            
            available_bins.insert(current_bin);
        } else {
            bins_result.emplace_back();
            size_t new_index = bins_result.size() - 1;
            
            Bin new_bin(batch_max_length - size, new_index);
            new_bin.items.push_back(orig_idx);
            available_bins.insert(new_bin);
        }
    }

    std::vector<std::vector<int>> final_result;
    final_result.resize(bins_result.size());
    for (const auto& bin : available_bins) {
        final_result[bin.bin_index] = bin.items;
    }

    return final_result;
}

PYBIND11_MODULE(ffd, m) {
    m.doc() = "FFD algorithm implementation in C++";
    m.def("ffd", &ffd, "FFD algorithm");
}