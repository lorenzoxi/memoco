#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <iostream>
using namespace std;

class Board {

private:
    int width;
    int height;
    int number_of_nodes;
    int number_of_nodes_placed;
    vector<tuple<int, int, int>> holes;
    vector<vector<bool>> board;


public:

    Board(const int &width, const int &height, const int &number_of_nodes);
    explicit Board(const vector<vector<bool>> & board);

    void set_board(const vector<vector<bool>> &);
    void set_width(int);
    void set_height(int);

    int get_width() const;
    int get_height() const;
    vector<vector<bool>> get_board() const;
    vector<tuple<int, int,int>> get_holes() const;
    int get_holes_size() const;

    bool can_place(int, int, int, int);
    bool try_to_place_component(int, int, int, int);
    void generate_nodes();
    vector<vector<double>> get_distance_matrix() const;

    void print_board() const;
    void print_holes() const;
};


#endif //BOARD_H
