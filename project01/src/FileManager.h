#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <vector>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;


class FileManager {

private:
    vector<vector<double>> distance_matrix;
    vector<vector<bool>> board_matrix;

public:
    FileManager();

    explicit FileManager(vector<vector<double>> distance_matrix, vector<vector<bool>> board_matrix = {});

    vector<vector<double>> get_distance_matrix() const;

    void set_distance_matrix(const vector<vector<double>> &distance_matrix);

    void write(const std::string& filename);

    std::vector<std::vector<double>> read(const std::string& filename, const std::string& type);
};



#endif //FILEMANAGER_H
