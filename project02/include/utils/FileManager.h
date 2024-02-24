#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <vector>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;


class FileManager {

private:
    string path;
    vector<vector<double>> distance_matrix;
    vector<vector<bool>> board_matrix;

public:
    FileManager();

    explicit FileManager(vector<vector<double>> distance_matrix, vector<vector<bool>> board_matrix = {}, string path = "../data");

    vector<vector<double>> get_distance_matrix() const;

    void set_distance_matrix(const vector<vector<double>> &distance_matrix);

    void write(const std::string& filename);

    std::vector<std::vector<double>> read_distance_matrix(const std::string& filename);

    std::vector<std::vector<bool>> read_board_matrix(const std::string& filename);
};



#endif //FILEMANAGER_H
