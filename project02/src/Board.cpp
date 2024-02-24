#include "../include/Board.h"
#include <random>
#include <tuple>

Board::Board(const vector<vector<bool>> & board) {
    this->board = board;
    this->width = board.size();
    this->height = board[0].size();
    this->holes = vector<tuple<int, int, int>>();
    this->number_of_nodes = 0;

    //generate the holes
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if(this->board[i][j]) {
                holes.emplace_back(i, j, number_of_nodes);
                number_of_nodes++;
            }
        }
    }
    this-> number_of_nodes_placed = number_of_nodes;
}



Board::Board(const int &width, const int &height, const int &number_of_nodes) {
    this->width = width;
    this->height = height;
    this->number_of_nodes = number_of_nodes;
    this->number_of_nodes_placed = 0;
    this->holes = vector<tuple<int, int, int>>();
    this->board = vector<vector<bool>>(width, vector<bool>(height, false));
}


void Board::set_board(const vector<vector<bool> > &board) {
    this->board = board;
}

void Board::set_width(const int dim_x) {
    this->width = dim_x;
}

void Board::set_height(const int dim_y) {
    this->height = dim_y;
}

int Board::get_width() const {
    return this->width;
}

int Board::get_height() const {
    return this->width;
}

vector<vector<bool> > Board::get_board() const {
    return this->board;
}

vector<tuple<int, int,int>> Board::get_holes() const {
    return this->holes;
}

int Board::get_holes_size() const {
    return this->holes.size();
}

bool Board::can_place(int x_coord, int y_coord, int component_width, int component_height) {

    if (x_coord + component_width >= width || y_coord + component_height >= height || x_coord < 0 || y_coord < 0) {
        return false;
    }

    //check if the component can be placed in the board
    for (int i = x_coord; i <= x_coord + component_width; i++) {
        for (int j = y_coord; j <= y_coord + component_height; j++) {
            if (board[i][j]) {
                return false;
            }
        }
    }

    return true;
}

bool Board::try_to_place_component(int x, int y, int component_width, int component_height) {

    vector<vector<int>> rotations = {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}};

    bool placed = false;

    for (int i=0; i<4 && !placed; i++) {
        const int x_rotation = rotations[i][0];
        const int y_rotation = rotations[i][1];

        // implicitely by default the component is placed in the same position (rotation 1, 1)
        int start_x = x;
        int start_y = y;
        int end_x = x + component_width;
        int end_y = y + component_height;

        if(x_rotation == 1 && y_rotation == -1) {
            start_x = x;
            end_x = x + component_width;
            start_y = y - component_height;
            end_y = y;
        }
        if(x_rotation == -1 && y_rotation == -1) {
            start_x = x - component_width;
            end_x = x;
            start_y = y - component_height;
            end_y = y;
        }
        if(x_rotation == -1 && y_rotation == 1) {
            start_x = x - component_width;
            end_x = x;
            start_y = y;
            end_y = y + component_height;
        }

        bool can_be_placed = can_place(x, y, component_width, component_height);
        if(can_be_placed) {
            placed = true;
            for (int w = start_x; w <= end_x && number_of_nodes_placed<number_of_nodes; w++) {
                for (int h = start_y; h <= end_y && number_of_nodes_placed<number_of_nodes; h++) {

                    //print only the border
                    if(w == start_x || w == end_x || h == start_y || h == end_y) {
                        board[w][h] = true;
                        number_of_nodes_placed++;
                    }
                }
            }

            return placed;
        }
    }
    return placed;
}


void Board::generate_nodes() {

    //generate randomly height and width numbers
    random_device rd;
    mt19937 gen(rd());
    mt19937 genx(rd());
    mt19937 geny(rd());

    uniform_int_distribution<> dis(2, 5);

    uniform_int_distribution<> dis_width(0, width);
    uniform_int_distribution<> dis_height(0, height);

    bool terminate = false;

    for (int i = 0; !terminate && number_of_nodes_placed<number_of_nodes; i++) {

        int component_width = dis(gen);
        int component_heigth = dis(gen);
        bool is_placed = false;

        int t = 0;
        int number_of_nodes_placed = 0;
        int number_of_tries = 2500;

        while(!is_placed && t<number_of_tries && number_of_nodes_placed<number_of_nodes) {
            int x = dis_width(genx);
            int y = dis_height(geny);
            is_placed = try_to_place_component(x, y, component_width, component_heigth);
            t++;
        }

        if(t==number_of_tries) {
            terminate = true;
        }
    }

    int counter = 0;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if(board[i][j]) {
                holes.emplace_back(i, j, counter);
                counter++;
            }
        }
    }
}

vector<vector<double>> Board::get_distance_matrix() const {

    vector<vector<double>> distance_matrix(holes.size(), vector<double>(holes.size(), 0));

    for(int i=0; i<holes.size(); i++) {
        for (int j=0; j<holes.size(); j++) {

                const int x1_coord = get<0>(holes[i]);
                const int y1_coord = get<1>(holes[i]);

                const int x2_coord = get<0>(holes[j]);
                const int y2_coord = get<1>(holes[j]);

                const pair<int, int> p_1(x1_coord, y1_coord);

                const pair<int, int> p_2(x2_coord, y2_coord);

                const double euclidean_distance = (sqrt(pow(p_1.first - p_2.first, 2) + pow(p_1.second - p_2.second, 2)) * 100.00 ) / 100.00;

                std::cout << "Distance between point P(" << p_1.first << "," << p_1.second << ") and P(" << p_2.first << "," << p_2.second << ") is " << euclidean_distance << std::endl;
                //print the calculus
                std::cout << pow(p_1.first - p_2.first, 2) << " + " << pow(p_1.second - p_2.second, 2) << " = " << pow(p_1.first - p_2.first, 2) + pow(p_1.second - p_2.second, 2) << std::endl;
                std::cout << "sqrt(" << pow(p_1.first - p_2.first, 2) + pow(p_1.second - p_2.second, 2) << ") = " << euclidean_distance << std::endl;

                distance_matrix[i][j] = euclidean_distance;
                distance_matrix[j][i] = euclidean_distance;

        }
    }

    return distance_matrix;
}

void Board::print_board() const {
    int count = 0;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if (board[i][j]) {
                cout << "⏺ ";
                count ++;
            } else {
                cout << "○ ";
            }
        }
        cout << endl;
    }
    cout << "Number of holes: " << count << endl;
}

void Board::print_holes() const {
    for (int i = 0; i < holes.size(); i++) {
        const int x = get<0>(holes[i]);
        const int y = get<1>(holes[i]);
        const int id = get<2>(holes[i]);
        cout << "P_" << id << "(" << x << "," << y << ")" <<endl;
    }
}
