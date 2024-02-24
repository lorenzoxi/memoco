#ifndef TESTING_H
#define TESTING_H
#include <vector>
#include <filesystem>
#include "../../include/schemes/SelectionScheme.h"
#include "../../include/schemes/TerminationScheme.h"
#include "../../include/schemes/MutationScheme.h"
#include "../schemes/InitScheme.h"

void run_genetic_algorithm_over_instances(
    const int &round,
    const InitScheme &initial_scheme,
    const int &population_size,
    const int &selection_size,
    SelectionScheme selection_scheme,
    const double &probability_mutation,
    MutationScheme mutation_scheme,
    TerminationScheme termination_scheme = TerminationScheme::MAX_NUMBER_GENERATIONS,
    const int &termination_max_generations = 100,
    const int &termination_max_not_improvement = 10,
    const int &termination_max_seconds = 300,
    const std::vector<int> &data_instances = {10,25,50,100,500},
    const std::string &other_info = "",
    const std::string& data_folder_path = "../data",
    const std::string &results_folder_path = "../results"
    );

std::string current_timestamp();

#endif //TESTING_H
