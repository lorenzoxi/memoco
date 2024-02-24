#include "../../include/utils/Run.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <chrono>
#include "../../include/utils/FileManager.h"
#include "../../include/Life.h"
#include "../../include/Generation.h"
#include "../../include/Individual.h"
#include "../../include/schemes/SelectionScheme.h"
#include "../../include/schemes/TerminationScheme.h"
#include "../../include/schemes/MutationScheme.h"
#include <ctime>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;


std::string current_timestamp() {
    // Get current time as time_point
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_time_t);
    std::ostringstream timestamp_stream;
    timestamp_stream << std::put_time(&now_tm, "%Y%m%d_%H%M%S");
    return timestamp_stream.str();
}

void run_genetic_algorithm_over_instances(
    const int &round,
    const InitScheme &initial_scheme,
    const int &population_size,
    const int &selection_size,
    SelectionScheme selection_scheme,
    const double &probability_mutation,
    MutationScheme mutation_scheme,
    TerminationScheme termination_scheme,
    const int &termination_max_generations,
    const int &termination_max_not_improvement,
    const int &termination_max_seconds,
    const std::vector<int> &data_instances,
    const std::string &other_info,
    const std::string &data_folder_path,
    const std::string &results_folder_path) {
    FileManager f = FileManager();

    try {
        for (int i = 0; i < data_instances.size(); i++) {
            std::string path = data_folder_path + "/" + std::to_string(data_instances[i]);
            std::ofstream file(
                results_folder_path + "/" + other_info + "-" + std::to_string(data_instances[i]) + "-" +
                current_timestamp() + ".csv");
            std::vector<std::vector<std::string> > data_to_write = {};
            std::vector<std::vector<std::string> > data_to_write_generations_best_individuals = {};

            data_to_write.push_back({
                "round",
                "time",
                "file",
                "nodes",
                "cost",
                "initial_scheme",
                "population_size",
                "selection_size",
                "selection_scheme",
                "probability_mutation",
                "mutation_scheme",
                "termination_scheme",
                "termination_max_generations",
                "path",
            });

            data_to_write_generations_best_individuals.push_back({
                "generation",
                "time",
                "file",
                "nodes",
                "cost",
                "initial_scheme",
                "population_size",
                "selection_size",
                "selection_scheme",
                "probability_mutation",
                "mutation_scheme",
                "termination_scheme",
                "termination_max_generations",
                "path",
            });



            for (const auto &entry: fs::directory_iterator(path)) {
                if (fs::is_regular_file(entry.status())) {

                    if (entry.path().filename().string().find("distance-matrix") != std::string::npos) {
                        std::cout << entry.path().filename() << std::endl;
                        std::string filename = entry.path().filename().string();
                        auto dotPosition = filename.find_last_of('.');
                        std::string instance_number_string = filename.substr(0, dotPosition);
                        std::string last_char = instance_number_string.substr(instance_number_string.size() - 1);
                        int instance_number = int (std::stoi(last_char));

                        const std::string file_to_read = path + "/" + entry.path().filename().string();
                        vector<vector<double> > distance_matrix = f.read_distance_matrix(file_to_read);
                        Life life = Life(
                            distance_matrix,
                            population_size,
                            initial_scheme,
                            selection_size,
                            selection_scheme,
                            probability_mutation,
                            mutation_scheme,
                            termination_scheme,
                            termination_max_generations,
                            termination_max_not_improvement,
                            termination_max_seconds,
                            instance_number,
                            round
                        );

                        auto start = std::chrono::high_resolution_clock::now();
                        life.start();
                        auto end = std::chrono::high_resolution_clock::now();
                        std::chrono::duration<double> elapsed = end - start;


                        const Individual best_individual = life.get_generations().back().get_individuals()[life.
                            get_generations().back().get_best_individual_index()];

                        std::vector<std::string> row = {
                            std::to_string(round),
                            std::to_string(elapsed.count()),
                            entry.path().filename().string(),
                            std::to_string(distance_matrix.size()),
                            std::to_string(best_individual.get_path_cost()),
                            std::to_string(initial_scheme),
                            std::to_string(population_size),
                            std::to_string(selection_size),
                            std::to_string(selection_scheme),
                            std::to_string(probability_mutation),
                            std::to_string(mutation_scheme),
                            std::to_string(termination_scheme),
                            std::to_string(termination_max_generations),
                            best_individual.path_to_string()
                        };
                        data_to_write.push_back(row);
                    }
                }
            }

            for (const auto &row: data_to_write) {
                for (size_t i = 0; i < row.size(); ++i) {
                    file << row[i];
                    if (i < row.size() - 1) file << ","; // Comma for all but last column
                }
                file << "\n"; // Newline at the end of each row
            }
            std::cout << "File written: " << results_folder_path + "/results-random-start-" + other_info +
                    std::to_string(data_instances[i]) + "-" + current_timestamp() + ".csv" << std::endl;
            data_to_write.clear();
            data_to_write.shrink_to_fit();
        }
    } catch (fs::filesystem_error &e) {
        std::cerr << e.what() << std::endl;
        return;
    }
}
