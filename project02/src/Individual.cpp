#include "../include/Individual.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <random>


std::mt19937 Individual::gen((std::random_device())());

Individual::Individual(const vector<double> &chromosomes) {
    this->chromosomes.reserve(chromosomes.size());
    this->chromosomes = chromosomes;
    this->fitness = 0;
    this->probability = 0;
    this->path_cost = 0;
}


Individual::Individual() {
    this->chromosomes = vector<double>();
    this->fitness = 0;
    this->probability = 0;
    this->path_cost = 0;
}

void Individual::set_fitness(const double &fitness) {
    this->fitness = fitness;
}

double Individual::get_fitness() const {
    return this->fitness;
}

vector<double>Individual::get_chromosomes() const {
    return this->chromosomes;
}

vector<double> & Individual::get_chromosomes_ref() {
    return this->chromosomes;
}




void Individual::set_chromosomes(const vector<double> &chromosome) {
    this->chromosomes = chromosome;
}

ostream &operator<<(ostream &os, const Individual &individual) {
    os << "Individual: [";
    for (const double chromosome: individual.chromosomes) {
        os << chromosome << " ";
    }
    os << "] - Fitness: " << individual.fitness << " - Probability: " << individual.probability << " - Path cost: "
            << individual.path_cost;
    return os;
}

bool Individual::operator<(const Individual &other) const {
    return (this->fitness < other.fitness);
}

void Individual::local_search(const int &max_iterations) {
    double best_fitness = this->fitness;

    for (int i = 0; i < max_iterations; i++) {
        //select randomly two chromosomes
        std::uniform_int_distribution<> dist_parent(0, this->chromosomes.size() - 1);

        const int start = dist_parent(gen);
        int end;
        do {
            end = dist_parent(gen);
        } while (start == end);


        //swap the two chromosomes and calculate the new fitness
        vector<double> new_chromosomes = this->chromosomes;
        std::swap(new_chromosomes[start], new_chromosomes[end]);
        std::reverse(new_chromosomes.begin() + start, new_chromosomes.begin() + end);


        Individual new_individual = Individual(new_chromosomes);
        new_individual.calculate_fitness_and_path_cost();

        if (new_individual.get_fitness() > best_fitness) {
            best_fitness = new_individual.get_fitness();
            this->chromosomes = new_chromosomes;
            this->fitness = best_fitness;
            this->path_cost = new_individual.get_path_cost();
        }
    }
}

void Individual::add_chromosomes(const double &chromosome) {
    this->chromosomes.push_back(chromosome);
}

void Individual::calculate_fitness_and_path_cost() {
    calculate_path_cost();
    this->fitness = 1 / (path_cost);
}

void Individual::set_path_cost(const double &path_cost) {
    this->path_cost = path_cost;
}

double Individual::get_path_cost() const {
    return this->path_cost;
}

void Individual::set_probability(const double &probability) {
    this->probability = probability;
}

double Individual::get_probability() const {
    return this->probability;
}

int Individual::get_chromosomes_size() const {
    return this->chromosomes.size();
}

void Individual::generate_chromosomes() {
    std::vector<int> numbers = std::vector<int>(Individual::distance_matrix.size());

    std::iota(numbers.begin(), numbers.end(), 0);

    // Use a random number generator to shuffle the vector
    std::random_device rd;
    std::mt19937 g(rd());
    // ranges::shuffle(numbers.begin(), numbers.end(), g);   //valid only for c++ >= 20
    std::shuffle(numbers.begin(), numbers.end(), g);

    this->chromosomes = vector<double>(numbers.begin(), numbers.end());
}

double Individual::calculate_path_cost() {
    int n = this->chromosomes.size();
    double path_cost= 0;

    for (int i = 0; i < n-1; i++) {
        int start = static_cast<int>(this->chromosomes[i]);
        int end = static_cast<int>(this->chromosomes[i+1]);
        path_cost += distance_matrix[start][end];
    }
    const int start = static_cast<int>(this->chromosomes[0]);
    const int end = static_cast<int>(this->chromosomes[n-1]);
    path_cost += distance_matrix[start][end];
    this->path_cost = path_cost;
    return path_cost;
}

void Individual::two_opt() {
    int n = this->chromosomes.size();

    double best_cost = calculate_path_cost();

    for (int i = 0; i < n - 1; i++) {
        for (int k = i + 1; k < n; k++) {
            Individual new_individual = Individual(this->chromosomes);
            std::reverse(this->chromosomes.begin() + i, this->chromosomes.begin() + k);
            double new_cost = new_individual.calculate_path_cost();

            if (new_cost < best_cost) {
                this->chromosomes = new_individual.get_chromosomes();
                best_cost = new_cost;
            }
        }
    }
}

std::string Individual::path_to_string() const {
    std::string path;
    for (const double chromosome: this->chromosomes) {
        path += std::to_string(static_cast<int>(chromosome));
        if (chromosome != this->chromosomes.back()) {
            path += "-";
        }

    }
    return path;
}



vector<vector<double> > Individual::distance_matrix = vector<vector<double> >();
