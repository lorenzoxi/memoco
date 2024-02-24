#include "../include/Generation.h"
#include <iostream>
#include <vector>
#include <random>
#include <numeric>
#include <algorithm>


std::mt19937 Generation::gen((std::random_device())());

Generation::Generation(const vector<Individual> &individuals) {
    this->individuals = individuals;
    this->size = individuals.size();
    this->generation_number = 0;
    this->fitness_sum = 0;
    this->best_individual_index = -1;
}

Generation::Generation(const vector<Individual> &individuals, const int &generation_number) {
    double minFitness = 0;

    for (int i = 0; i < individuals.size(); i++) {
        minFitness = individuals[i].get_fitness();

        if (individuals[i].get_fitness() < minFitness) {
            this->best_individual_index = i;
        }
        this->individuals.push_back(individuals[i]);
    }

    this->size = individuals.size();
    this->generation_number = generation_number;
    normalize_fitness();
    calculate_fitness_sum();
}

Generation::Generation() {
    this->individuals = vector<Individual>();
    this->size = 0;
    this->generation_number = 0;
}

void Generation::set_individuals(const vector<Individual> &individuals) {
    this->individuals = individuals;
}

vector<Individual> Generation::get_individuals() const {
    return this->individuals;
}

void Generation::set_generation_number(const int &generation_number) {
    this->generation_number = generation_number;
}

int Generation::get_generation_number() const {
    return this->generation_number;
}

void Generation::set_fitness_sum(const double &fitness_sum) {
    this->fitness_sum = fitness_sum;
}

double Generation::get_fitness_sum() const {
    return this->fitness_sum;
}

void Generation::calculate_fitness_sum() {
    this->fitness_sum = 0;
    for (const auto &individual: this->individuals) {
        this->fitness_sum += individual.get_fitness();
    }
}

ostream &operator<<(ostream &os, const Generation &generation) {
    os << "Generation: " << generation.generation_number << endl;
    for (const auto &individual: generation.individuals) {
        os << individual << endl;
    }
    return os;
}

vector<Individual> Generation::selection(const SelectionScheme &selectionScheme, const int &individualsToSelect) {
    normalize_fitness();
    calculate_fitness_sum();
    std::vector<Individual> selected_parents;



    switch (selectionScheme) {
        case MONTECARLO: {
            //set probability of all individuals according to the formula fitness/sum(fitness)
            for (auto &individual: this->individuals) {
                individual.set_probability(individual.get_fitness() / this->fitness_sum);
            }

            //pick n individuals according to their probability
            std::vector<double> probabilities;

            probabilities.reserve(individuals.size());
            std::transform(individuals.begin(), individuals.end(), std::back_inserter(probabilities),
                           [](const Individual &ind) {
                               return ind.get_probability();
                           });

            std::vector<double> cumulative;
            cumulative.reserve(probabilities.size());
            std::partial_sum(probabilities.begin(), probabilities.end(), std::back_inserter(cumulative));

            std::default_random_engine generator;
            std::uniform_real_distribution<double> distribution(0.0, 1.0);

            while (selected_parents.size() < individualsToSelect) {
                double randomValue = distribution(generator);
                auto it = std::lower_bound(cumulative.begin(), cumulative.end(), randomValue);
                int index = std::distance(cumulative.begin(), it);
                Individual selected = this->individuals[index];
                selected_parents.push_back(selected);
            }

            return selected_parents; //end MONTECARLO selection
        }
        case TOURNAMENT: {
            selected_parents.reserve(individualsToSelect);
            std::uniform_int_distribution<> dis(0, this->individuals.size() - 1);

            for (int i = 0; i < individualsToSelect; i++) {
                std::vector<Individual> tournament;
                tournament.reserve(5);
                for (int j = 0; j < 5; j++) {
                    int index = dis(gen);
                    tournament.push_back(this->individuals[index]);
                }
                std::sort(tournament.begin(), tournament.end(), [](const Individual &a, const Individual &b) {
                    return a.get_fitness() > b.get_fitness();
                });
                selected_parents.push_back(tournament[0]);
            }
            return selected_parents;
        }
    }
}

vector<Individual> Generation::crossover(const vector<Individual> &individuals, const int &children_number) {
    vector<Individual> children;
    children.reserve(children_number);

    //select among the individuals
    const int parent_size = individuals[0].get_chromosomes_size();

    //generate two random different random numbers between 0 and parent_size
    std::uniform_int_distribution<> dist_cutoff(0, parent_size - 1);
    std::uniform_int_distribution<> dist_parent(0, individuals.size() - 1);

    vector<double> child_chromosomes;

    for (int n = 0; n < children_number; n++) {
        //select two different parents
        const int parent_1_index = dist_parent(gen);
        int parent_2_index;
        do {
            parent_2_index = dist_parent(gen);
        } while (parent_1_index == parent_2_index);


        const Individual parent_1 = individuals[parent_1_index];
        vector<double> parent_1_chromosomes = parent_1.get_chromosomes();
        const Individual parent_2 = individuals[parent_2_index];
        vector<double> parent_2_chromosomes = parent_2.get_chromosomes();

        child_chromosomes.clear();


        const int cut_off_point_1 = dist_cutoff(gen);
        int cut_off_point_2;

        do {
            cut_off_point_2 = dist_cutoff(gen);
        } while (cut_off_point_1 == cut_off_point_2);

        // copy the genes of parent 1 between the cut off points to child 2 and vice versa
        for (int i = cut_off_point_1; i <= cut_off_point_2; i++) {
            child_chromosomes.push_back(parent_2_chromosomes[i]);
        }

        // copy the genes before the first cut off point and avoid duplicates
        for (int i = 0; i < cut_off_point_1; i++) {
            //if the gene is not in the child add it
            if (std::find(child_chromosomes.begin(), child_chromosomes.end(), parent_1_chromosomes[i]) ==
                child_chromosomes.end()) {
                child_chromosomes.push_back(parent_1_chromosomes[i]);
            } else {
                // if the gene is already in the child, add the first parent gene that is not in the child (avoid duplicates)
                for (int j = 0; j < parent_size; j++) {
                    if (std::find(child_chromosomes.begin(), child_chromosomes.end(), parent_1_chromosomes[j]) ==
                        child_chromosomes.end()) {
                        child_chromosomes.push_back(parent_1_chromosomes[j]);
                        break;
                    }
                }
            }
        }

        // copy the genes after the second cut off point and avoid duplicates
        for (int i = cut_off_point_2 + 1; i < parent_size; i++) {
            if (std::find(child_chromosomes.begin(), child_chromosomes.end(), parent_1_chromosomes[i]) ==
                child_chromosomes.end()) {
                child_chromosomes.push_back(parent_1_chromosomes[i]);
            } else {
                for (int j = 0; j < parent_size; j++) {
                    if (std::find(child_chromosomes.begin(), child_chromosomes.end(), parent_1_chromosomes[j]) ==
                        child_chromosomes.end()) {
                        child_chromosomes.push_back(parent_1_chromosomes[j]);
                        break;
                    }
                }
            }
        }

        Individual child = Individual(child_chromosomes);

        child.local_search(100);
        child.calculate_fitness_and_path_cost();
        children.push_back(child);
    }


    return children;
}

void Generation::mutation(const MutationScheme &mutationScheme) {
    //generate a random number between 0 and individuals.size() - 1 to select the individual to mutate
    std::uniform_int_distribution<> dis(0, this->individuals.size() - 1);

    //select the individual to mutate and shuffle its genes
    int individual_to_mutate = dis(gen);

    auto begin = this->individuals[individual_to_mutate].begin();
    auto end = this->individuals[individual_to_mutate].end();

    if (mutationScheme == MUTATION_SHUFFLE) {
        std::cout << "Shuffle mutation" << std::endl;
        std::shuffle(begin, end, gen);

    } else if (mutationScheme == MUTATION_2OPT_MOVE) {
        //2-opt mutation
        //generate two different random numbers between 0 and parent_size
        std::uniform_int_distribution<> dist(0, this->individuals[individual_to_mutate].get_chromosomes_ref().size() - 1);
        int i = dist(gen);
        int j;
        do {
            j = dist(gen);
        } while (i == j);

        // make a 2-opt move
        std::reverse(begin + i, begin + j);
    }
}

int Generation::get_best_individual_index() {
    double maxFitness = 0;

    for (int i = 0; i < this->individuals.size(); i++) {
        const double fitness = this->individuals[i].get_fitness();
        if (fitness > maxFitness) {
            this->best_individual_index = i;
            maxFitness = fitness;
        }
    }
    return this->best_individual_index;
}

double Generation::get_best_individual_cost() {
    return this->individuals[get_best_individual_index()].get_path_cost();
}

void Generation::normalize_fitness() {
    // fint max and min fitness
    double maxFitness = 0;
    double minFitness = 0;
    for (int i = 0; i < this->individuals.size(); i++) {
        if (this->individuals[i].get_fitness() > maxFitness) {
            maxFitness = this->individuals[i].get_fitness();
        }
        if (this->individuals[i].get_fitness() < minFitness) {
            minFitness = this->individuals[i].get_fitness();
        }
    }

    // normalize fitness
    for (int i = 0; i < this->individuals.size(); i++) {
        double normalizedFitness = (this->individuals[i].get_fitness() - minFitness) / (maxFitness - minFitness);
        this->individuals[i].set_fitness(normalizedFitness);
    }
}
