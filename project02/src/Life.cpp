#include "../include/Life.h"
#include <iostream>
#include <random>
#include <iomanip>
#include <chrono>
#include <thread>
#include <fstream>
#include <experimental/filesystem>
using namespace std;
namespace fs = std::experimental::filesystem;

using namespace std;
std::mt19937 Life::gen((std::random_device())());

Life::Life(
    const vector<vector<double> > &distance_matrix,
    const int &population_size,
    InitScheme initial_scheme,
    const int &selection_size,
    SelectionScheme selection_scheme,
    const double &probability_mutation,
    MutationScheme mutation_scheme,
    TerminationScheme termination_scheme,
    const int &termination_max_generations,
    const int &termination_max_not_improvement,
    const int &termination_max_seconds,
    const int &instance_number,
    const int &round_time
)
    : population_size(population_size),
      selection_size(selection_size),
      selection_scheme(selection_scheme),
      init_scheme(initial_scheme),
      probability_mutation(probability_mutation),
      mutation_scheme(mutation_scheme),
      termination_scheme(termination_scheme),
      termination_max_generations(termination_max_generations),
      termination_max_not_improvement(termination_max_not_improvement),
      termination_max_seconds(termination_max_seconds),
      distance_matrix(distance_matrix),
      instance_number(instance_number),
      round_time(round_time) {
}

vector<Generation> Life::get_generations() const {
    return this->generations;
}

void Life::set_generations(const vector<Generation> &generations) {
    this->generations = generations;
}

int Life::get_max_generations() const {
    return this->termination_max_generations;
}

void Life::set_max_generations(const int &termination_max_generations) {
    this->termination_max_generations = termination_max_generations;
}

double Life::get_probability_mutation() const {
    return this->probability_mutation;
}

void Life::set_probability_mutation(const double &probability_mutation) {
    this->probability_mutation = probability_mutation;
}

int Life::get_population_size() const {
    return this->population_size;
}

void Life::set_population_size(const int &population_size) {
    this->population_size = population_size;
}

int Life::get_selection_size() const {
    return this->selection_size;
}

void Life::set_selection_size(const int &selection_size) {
    this->selection_size = selection_size;
}

SelectionScheme Life::get_selection_scheme() const {
    return this->selection_scheme;
}

void Life::set_selection_scheme(SelectionScheme selection_scheme) {
    this->selection_scheme = selection_scheme;
}

TerminationScheme Life::get_termination_scheme() const {
    return this->termination_scheme;
}

void Life::set_termination_scheme(TerminationScheme termination_scheme) {
    this->termination_scheme = termination_scheme;
}

void Life::start() {
    Individual::distance_matrix = this->distance_matrix;

    //1. generate initial population
    vector<Individual> v;
    for (int i = 0; i < this->population_size; i++) {
        Individual individual = Individual();
        individual.generate_chromosomes();
        v.push_back(individual);
    }


    //2. evaluate the fitness of each individual in the population
    for (Individual &individual: v) {
        individual.calculate_fitness_and_path_cost();
    }

    //3. make 2-opt local search in the initial population if needed
    if (init_scheme == InitScheme::INIT_2OPT) {
        for (Individual &individual: v) {
            individual.two_opt();
            individual.calculate_fitness_and_path_cost();
        }
    }

    this->generations.emplace_back(v, 0);
    bool terminate = false;
    bool performMutation = false;
    std::uniform_int_distribution<> distr(1, this->probability_mutation); // Define the range

    auto start = std::chrono::high_resolution_clock::now(); // Get the start time
    int not_improvement_gen = 0;

    /*
    // --- LOGGING --- only in c++ >= 20
    const int number_of_nodes = this->generations[0].get_individuals()[0].get_chromosomes().size();
    const string path = "../newdata1/results/generations/" + std::to_string(number_of_nodes);
    fs::create_directories(path);
    const string file = path + "/r" + std::to_string(round_time) + "instance-" + std::to_string(this->instance_number) + ".csv";
    //create a file to write the best individual of the generation
    ofstream file_to_write(file, ios::app);
    file_to_write << "round, nodes, instance, generation, time, cost" << endl;
    */

    for (int i = 0; terminate == false; i++) {
        auto end = std::chrono::high_resolution_clock::now(); // Get the current time
        std::chrono::duration<double> elapsed = end - start; // Get the elapsed time


        if (this->termination_scheme == TerminationScheme::MAX_NUMBER_SECONDS &&
            elapsed.count() >= termination_max_seconds) {
            terminate = true;
        } else {
            const vector<Individual> parents = generations[i].selection(selection_scheme, selection_size);

            const vector<Individual> &crossover = generations[i].crossover(parents, population_size);
            generations.emplace_back(crossover, i);

            int rand_num = distr(gen);
            if (rand_num == 1) {
                generations[i].mutation(mutation_scheme);
            }

            // cout << "> Generation " << i << endl;

            if (this->termination_scheme == TerminationScheme::MAX_NUMBER_GENERATIONS && i == this->
                termination_max_generations - 1) {
                terminate = true;
            }

            if (this->termination_scheme == TerminationScheme::MAX_NUMBER_NOT_IMPROVEMENT) {
                const int bi = generations[i].get_best_individual_index();
                const double best_i_cost = generations[i].get_best_individual_cost();

                /* --- LOGGING ---
                // write the best individual of the generation
                file_to_write << round_time << "," << number_of_nodes << ", " << instance_number << ", " << i << ", " <<
                        elapsed.count() <<
                        ", " << best_i_cost << endl;
                */

                if (i == 0) {
                    not_improvement_gen = 0;
                } else {
                    const double best_i_1_cost = generations[i - 1].get_best_individual_cost();
                    if (best_i_cost >= best_i_1_cost) {
                        not_improvement_gen++;
                        //std::cout << "  Not improved generation " << i << " - " << not_improvement_gen << " - " <<
                        // generations[i].get_best_individual_cost() << std::endl;
                    } else {
                        not_improvement_gen = 0;
                        //std::cout << "Improved in generation " << i << " - " << generations[i].
                        //        get_best_individual_cost() << std::endl;
                    }
                }


                if (not_improvement_gen == this->termination_max_not_improvement) {
                    terminate = true;
                    // delete the last non improved generation
                    generations.erase(generations.end() - not_improvement_gen, generations.end());
                    //print the last best found
                    auto it = generations.end() -1;
                    auto best = *it;
                    std::cout << best.get_best_individual_cost() << std::endl;
                }
            }
        }

        //cout << "-----------------------------------" << endl;
    }
    //file_to_write.close();    //logging in c++ >=20
}


void Life::print_distance_matrix() const {
    for (const vector<double> &row: this->distance_matrix) {
        for (const double &distance: row) {
            std::cout << std::setw(6) << std::setprecision(2) << std::fixed << distance << " ";
        }
        cout << endl;
    }
}
