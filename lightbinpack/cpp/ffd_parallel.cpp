#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <set>
#include <algorithm>
#include <memory>
#include <omp.h>
#include <mutex>

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

    const size_t chunk_size = std::max(size_t(1000), length_pairs.size() / (omp_get_max_threads() * 4));
    std::vector<std::vector<std::vector<int>>> thread_results(omp_get_max_threads());

    #pragma omp parallel
    {
        const int thread_id = omp_get_thread_num();
        std::set<Bin> local_bins;

        #pragma omp for schedule(dynamic, chunk_size)
        for(size_t i = 0; i < length_pairs.size(); i++) {
            const auto& pair = length_pairs[i];
            double size = pair.first;
            int orig_idx = pair.second;

            auto it = local_bins.lower_bound(Bin(size, 0));
            
            if (it != local_bins.end()) {
                Bin current_bin = *it;
                local_bins.erase(it);
                
                current_bin.remaining_space -= size;
                current_bin.items.push_back(orig_idx);
                
                local_bins.insert(current_bin);
            } else {
                size_t new_index = thread_results[thread_id].size();
                thread_results[thread_id].emplace_back();
                
                Bin new_bin(batch_max_length - size, new_index);
                new_bin.items.push_back(orig_idx);
                local_bins.insert(new_bin);
            }
        }

        thread_results[thread_id].resize(thread_results[thread_id].size());
        for (const auto& bin : local_bins) {
            thread_results[thread_id][bin.bin_index] = bin.items;
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