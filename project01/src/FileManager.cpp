//
// Created by Lorenzo Perinello on 13/02/24.
//

#include "FileManager.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>

FileManager::FileManager() {
    this->distance_matrix = {};
}

FileManager::FileManager(vector<vector<double>> distance_matrix, vector<vector<bool>> board_matrix) {
    this->distance_matrix = distance_matrix;
    this->board_matrix = board_matrix;
}

vector<vector<double>> FileManager::get_distance_matrix() const {
    return this->distance_matrix;
}

void FileManager::set_distance_matrix(const vector<vector<double>> &distance_matrix) {
    this->distance_matrix = distance_matrix;
}

void FileManager::write(const std::string& filename) {
    std::string path = "../data/distance-" + filename + ".txt";
    std::ofstream outFile(path);
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


    path = "../data/board-" + filename + ".txt";
    std::ofstream outFile_1(path);
    if (!outFile_1.is_open()) {
        std::cerr << "Error opening file for writing\n";
        return;
    }

    for(int i = 0; i < board_matrix.size(); i++) {
        for (int j = 0; j < board_matrix[i].size(); j++) {
            outFile_1 << board_matrix[i][j] << " ";
        }
        outFile_1 << std::endl;
    }

    outFile.close();
}

std::vector<std::vector<double>> FileManager::read(const std::string& filename, const std::string& type) {

    std::ifstream inFile(filename);

    if (type == "distance") {
        std::vector<std::vector<double>> matrix;
    } else if (type == "board") {
        std::vector<std::vector<bool>> matrix;
    } else {
        std::cerr << "Error: type not recognized\n";
        return {};
    }
    std::vector<std::vector<double>> matrix;
    if (!inFile.is_open()) {
        std::cerr << "Error opening file for reading\n";
        return matrix;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);

        auto row = std::vector<double>();

        if (type == "board") {
            std::vector<bool> row;
        } else if (type == "distance") {
            std::vector<double> row;
        }
        double val;
        while (iss >> val) {
            row.push_back(val);
        }
        matrix.push_back(row);
    }

    inFile.close();

    return matrix;

}
