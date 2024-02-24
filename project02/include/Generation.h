#ifndef GENERATION_H
#define GENERATION_H

#include <random>
#include <vector>
#include "Individual.h"
#include "schemes/SelectionScheme.h"
#include "schemes/MutationScheme.h"

class Generation
{
private:
  vector<Individual> individuals;
  unsigned long size;
  int generation_number;
  double fitness_sum;
  int best_individual_index;
  static std::mt19937 gen;

public:
  explicit Generation(const vector<Individual>& individuals);
  Generation(const vector<Individual>& individuals, const int &generation_number);
  Generation();

  void set_individuals(const vector<Individual> &individuals);
  vector<Individual> get_individuals() const;

  void set_generation_number (const int &generation_number);
  int get_generation_number() const;

  void set_fitness_sum(const double &fitness_sum);
  double get_fitness_sum() const;
  void calculate_fitness_sum();

  void normalize_fitness();

  friend ostream &operator<<(ostream &os, const Generation &generation);

  vector<Individual> selection(const SelectionScheme &selectionScheme, const int &individualsToSelect);
  vector<Individual> crossover(const vector<Individual> &elite, const int &children_number);
  void mutation(const MutationScheme &mutationScheme);
  int get_best_individual_index();
  double get_best_individual_cost();


};

#endif //GENERATION_H
