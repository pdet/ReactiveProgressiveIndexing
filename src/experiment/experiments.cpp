#include "experiment/experiments.hpp"
#include "progressive/progressive_quicksort.hpp"
#include <iostream>
#include <string>
using namespace std;
using namespace chrono;

Experiments::Experiments(unique_ptr<vector<int64_t>> column, unique_ptr<vector<RangeQuery>> queries, unique_ptr<vector<Algorithm>> algorithms)
    : column(move(column)), queries(move(queries)), algorithms(move(algorithms)) {}

void Experiments::progressive_indexing_initialize() {

    if (interactivity_threshold == 0) {
        progressiveIndex = make_unique<ProgressiveQuicksort>(delta, test);
    } else {
        //! FIXME: hacky, should change the original-column representation
        unique_ptr<IdxCol> originalColumn = make_unique<IdxCol>(column->size());
        for (size_t i = 0; i < column->size(); i++) {
            originalColumn->data[i].m_rowId = i;
            originalColumn->data[i].m_key = (*column)[i];
        }
        auto startTimer = std::chrono::system_clock::now();
        int64_t sum = originalColumn->full_scan(10000, 11000);
        auto endTimer = std::chrono::system_clock::now();
        //! Avoid -O3 opt
        if (sum != 0) {
            fprintf(stderr, " ");
        }
        auto full_scan_time = duration<double, std::milli>(endTimer - startTimer).count();
        progressiveIndex = make_unique<ProgressiveQuicksort>(interactivity_threshold, full_scan_time, test);
    }
}
int64_t Experiments::progressive_indexing(size_t query_it) {
    auto low = queries->at(query_it).left_predicate;
    auto high = queries->at(query_it).right_predicate;
    return progressiveIndex->run_query(*column.get(), low, high, *time.get(), query_it);
}
void Experiments::run() {
    //! Initialize Profiling
    time = make_unique<std::vector<double>>(queries->size());
    //! Run Algorithms
    for (auto const& algorithm : *algorithms) {
        switch (algorithm.indexId) {
        case IndexId ::PROGRESSIVEINDEXING: {
            progressive_indexing_initialize();
            for (size_t i = 0; i < queries->size(); i++) {
                progressive_indexing(i);
            }
            break;
        }
        default:
            throw "Algorithm Does Not Exist";
        }
    }
}

int64_t Experiments::run_query(size_t query_it) {
    //! Initialize Profiling and update_counter if we are running first
    //! query
    if (query_it == 0) {
        progressive_indexing_initialize();
        time = make_unique<std::vector<double>>(queries->size());
    }
    for (auto const& algorithm : *algorithms) {
        switch (algorithm.indexId) {
        case IndexId ::PROGRESSIVEINDEXING: {
            return progressive_indexing(query_it);
        }

        default:
            throw "Algorithm Does Not Exist";
        }
    }
}
void Experiments::print_results() {
    for (size_t i = 0; i < time->size(); i++) {
        cout << time->at(i) << "\n";
    }
};
