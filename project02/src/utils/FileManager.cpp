#include "../../include/utils/FileManager.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <experimental/filesystem>
#include <iomanip>

using namespace std;
namespace fs = std::experimental::filesystem;

FileManager::FileManager() {
    this->path = "";
    this->distance_matrix = {};
}

FileManager::FileManager(vector<vector<double>> distance_matrix, vector<vector<bool>> board_matrix, string path) {
    this->distance_matrix = distance_matrix;
    this->board_matrix = board_matrix;
    this->path = path;
}

vector<vector<double>> FileManager::get_distance_matrix() const {
    return this->distance_matrix;
}

void FileManager::set_distance_matrix(const vector<vector<double>> &distance_matrix) {
    this->distance_matrix = distance_matrix;
}

void FileManager::write(const std::string& filename) {
    int size = distance_matrix.size();
    std::string dirpath = this->path + "/" +std::to_string(size);
    std::string full_path = dirpath + "/" + "distance-" + filename + ".txt";

    /* check if the directory exists, otherwise create it (only in c++ >= 20)
    if (!fs::exists(dirpath)) {
        fs::create_directories(dirpath);
    }
    */

    std::ofstream outFile(full_path);
    std::cout << full_path << std::endl;

    if (!outFile.is_open()) {
        std::cerr << "Error opening file for writing\n";
        return;
    }

    for(int i = 0; i < distance_matrix.size(); i++) {
        for (int j = 0; j < distance_matrix[i].size(); j++) {
            outFile << std::setw(10) << distance_matrix[i][j];
        }
        outFile << std::endl;
    }

    outFile.close();


    full_path = dirpath + "/" + "boolean-" + filename + ".txt";
    std::ofstream outFile_1(full_path);
    if (!outFile_1.is_open()) {
        std::cerr << "Error opening file for writing\n";
        return;
    }

    for(int i = 0; i < board_matrix.size(); i++) {
        for (int j = 0; j < board_matrix[i].size(); j++) {
            outFile_1 << board_matrix[i][j] << "";
        }
        outFile_1 << std::endl;
    }

    outFile.close();
}

std::vector<std::vector<double>> FileManager::read_distance_matrix(const std::string& filename) {
    std::ifstream inFile(filename);
    std::vector<std::vector<double>> matrix;
    if (!inFile.is_open()) {
        std::cerr << "Error opening file for reading\n";
        return matrix;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        auto row = std::vector<double>();
        double val;
        while (iss >> val) {
            row.push_back(val);
        }
        matrix.push_back(row);
    }

    inFile.close();
    return matrix;
}


std::vector<std::vector<bool>> FileManager::read_board_matrix(const std::string& filename) {
    std::ifstream inFile(filename);
    std::vector<std::vector<bool>> matrix;
    if (!inFile.is_open()) {
        std::cerr << "Error opening file for reading\n";
        return matrix;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        auto row = std::vector<bool>();
        for (const char ch : line) {
            row.push_back(ch == '1'); // '1' becomes true, '0' becomes false
        }
        matrix.push_back(row);
    }

    inFile.close();

    return matrix;
}