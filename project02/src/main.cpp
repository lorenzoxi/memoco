#include "../include/Board.h"
#include "../include/utils/FileManager.h"
#include "../include/Life.h"
#include "../include/Individual.h"
#include "../include/schemes/SelectionScheme.h"
#include "../include/schemes/TerminationScheme.h"
#include "../include/schemes/MutationScheme.h"
#include <chrono>
#include <vector>
#include <iostream>

int main() {

    // --- INSTANCE TO SOLVE CONFIGURATION ------
    int number_of_nodes = 25;   //!! number_of_nodes values available: 10, 25, 50, 100, (150)
    int instance_number = 0;    //!! instance_number values available: 0,1,2,3,4,5, (for 150 only 4)

    // read distance matrix (i.e. instance of a problem)
    FileManager f = FileManager();
    const std::string file_to_read = "data/" + std::to_string(number_of_nodes) + "/distance-matrix-"+ std::to_string(number_of_nodes) + "-" + std::to_string(instance_number) + ".txt";
    std::cout << file_to_read << std::endl;
    std::vector<std::vector<double> > distance_matrix = f.read_distance_matrix(file_to_read);

    // --- G.A. CONFIGURATION -------------------
    const int population_size = 100;
    const InitScheme initial_scheme = InitScheme::INIT_RANDOM;
    const int selection_size = 10;
    const SelectionScheme selection_scheme = SelectionScheme::TOURNAMENT;
    const int probability_mutation = 1000;
    const MutationScheme mutation_scheme = MutationScheme::MUTATION_SHUFFLE;
    const TerminationScheme termination_scheme = TerminationScheme::MAX_NUMBER_NOT_IMPROVEMENT;
    const int termination_max_generations = 100;
    const int termination_max_not_improvement = 8;
    const int termination_max_seconds = 100;
    const int round = 0;

    // --- Run G.A. algorithm --------------------
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

    life.start();
}
