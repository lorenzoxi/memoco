#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <random>
#include <vector>


using namespace std;

class Individual
{
private:
  vector<double> chromosomes;
  double fitness;
  double probability;
  double path_cost;
  static std::mt19937 gen;

public:
  static vector<vector<double>> distance_matrix;
  Individual();
  explicit Individual(const vector<double> &chromosomes);

  vector<double> get_chromosomes() const;
  vector<double> &get_chromosomes_ref();

  std::vector<double>::iterator begin() {
    return chromosomes.begin();
  }

  std::vector<double>::iterator end() {
    return chromosomes.end();
  }


  void set_chromosomes(const vector<double>& chromosome);
  void add_chromosomes(const double &chromosome);
  void generate_chromosomes();

  void set_fitness(const double &);
  double get_fitness() const;

  void set_path_cost(const double &);
  double get_path_cost() const;

  void set_probability(const double &);
  double get_probability() const;

  int get_chromosomes_size() const;

  void calculate_fitness_and_path_cost();

  friend ostream &operator<<(ostream &os, const Individual &elite);
  bool operator<(const Individual &other) const;
  double calculate_path_cost();

  std::string path_to_string() const;

  void two_opt();
  void local_search(const int& max_iterations);

};

#endif //INDIVIDUAL_H