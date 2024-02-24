#ifndef LIFE_H
#define LIFE_H
#include "Generation.h"
#include "schemes/SelectionScheme.h"
#include "schemes/TerminationScheme.h"
#include "schemes/MutationScheme.h"
#include "schemes/InitScheme.h"

class Life {
private:
    int termination_max_generations;
    double probability_mutation;
    int population_size;
    int selection_size;
    SelectionScheme selection_scheme;
    InitScheme init_scheme;
    MutationScheme mutation_scheme;
    TerminationScheme termination_scheme;
    vector<Generation> generations;
    vector<vector<double> > distance_matrix;
    int termination_max_not_improvement;
    int termination_max_seconds;
    int instance_number;
    int round_time;
    static std::mt19937 gen;

public:
    Life(
        const vector<vector<double> > &distance_matrix,
        const int &population_size,
        InitScheme initial_scheme,
        const int &selection_size,
        SelectionScheme selection_scheme,
        const double &probability_mutation,
        MutationScheme mutation_scheme,
        TerminationScheme termination_scheme = TerminationScheme::MAX_NUMBER_GENERATIONS,
        const int &termination_max_generations = 100,
        const int &termination_max_not_improvement = 10,
        const int &termination_max_seconds = 300,
        const int &instance_number = 0,
        const int &round_time = 0
        );

    vector<Generation> get_generations() const;
    void set_generations(const vector<Generation> &generations);

    int get_max_generations() const;

    void set_max_generations(const int &termination_max_generations);

    double get_probability_mutation() const;

    void set_probability_mutation(const double &probability_mutation);

    int get_population_size() const;

    void set_population_size(const int &population_size);

    int get_selection_size() const;

    void set_selection_size(const int &selection_size);

    SelectionScheme get_selection_scheme() const;

    void set_selection_scheme(SelectionScheme selection_scheme);

    TerminationScheme get_termination_scheme() const;

    void set_termination_scheme(TerminationScheme termination_scheme);

    void start();

    void print_distance_matrix() const;
};

#endif //LIFE_H
