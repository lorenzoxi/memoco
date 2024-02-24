#include "cpxmacro.h"
#include "FileManager.h"
#include <cstdio>
#include <iostream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <vector>
#include <experimental/filesystem>
#include <fstream>
#include <chrono>
#include <tuple>

using namespace std;
namespace fs = std::experimental::filesystem;

struct arc {
    int var_position;
    int start;
    int end;
};

std::string current_timestamp() {
    // Get current time as time_point
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_time_t);
    std::ostringstream timestamp_stream;
    timestamp_stream << std::put_time(&now_tm, "%Y%m%d_%H%M%S");
    return timestamp_stream.str();
}

std::string remove_txt_extension(const std::string &filename) {
    const std::string extension = ".txt";
    if (filename.size() >= extension.size() &&
        filename.compare(filename.size() - extension.size(), extension.size(), extension) == 0) {
        // Remove the ".txt" extension
        return filename.substr(0, filename.size() - extension.size());
    } else {
        return filename;
    }
}

std::string retrieve_cycle(std::vector<arc> &arcs) {
    int k = 0;
    std::vector<std::string> final_path;
    int first_node = arcs[0].start;
    while (arcs.size() > 0) {
        //add the first vertex of the arc
        int next_start = arcs[k].end;
        final_path.push_back(std::to_string(arcs[k].start));

        //delete the current arc
        arcs.erase(arcs.begin() + k);

        //search the arc which start is equal to the current arcs end
        bool find = false;
        for (auto it = arcs.begin(); it != arcs.end() && !find; ++it) {
            if (it->start == next_start) {
                find = true;
                k = std::distance(arcs.begin(), it);
            }
        }
    }
    final_path.push_back(std::to_string(first_node));

    std::string path_to_save = "[";
    for (int i = 0; i < final_path.size(); ++i) {
        path_to_save += final_path[i];
        if (i < final_path.size() - 1) {
            path_to_save += ",";
        }
    }
    path_to_save += "]";

    std::cout << "Best cycle found: " << path_to_save << std::endl;
    return path_to_save;
}

std::vector<arc> retrieve_arcs_from_solution(std::vector<double> &var_vals, const std::vector<arc> &ys) {
    std::vector<arc> arcs;
    for (int i = 0; i < ys.size(); ++i) {
        int pos = ys[i].var_position;
        if (var_vals[pos] > 0.5) {
            // ==> y_ij == 1
            arcs.push_back(ys[i]);
        }
    }
    return arcs;
}


int status;
char errmsg[BUF_SIZE];

void setupLP(CEnv env, Prob lp, int &current_var_position, const vector<vector<double> > &distance_matrix,
             std::vector<arc> &ys, std::vector<arc> &xs, std::string timestamp, int lpcount) {
    current_var_position = 0;
    const int N = distance_matrix.size();

    // ------------------------------------------------------------
    // --------------- MAPS
    // ------------------------------------------------------------

    // map_x[i][j] = x_ij
    vector<vector<int> > map_x;
    map_x.resize(N);
    for (int i = 0; i < N; ++i) {
        map_x[i].resize(N);
        for (int j = 0; j < N; ++j) {
            map_x[i][j] = -1;
        }
    }
    // map_y[i][j] = y_ij
    vector<vector<int> > map_y;
    map_y.resize(N);
    for (int i = 0; i < N; ++i) {
        map_y[i].resize(N);
        for (int j = 0; j < N; ++j) {
            map_x[i][j] = -1;
        }
    }

    // ------------------------------------------------------------
    // --------------- VARIABLES
    // ------------------------------------------------------------

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (i == j) {
                continue;
            }
            char ytype = 'B';
            double lb = 0.0;
            double ub = 1.0;
            std::string name = "y_" + std::to_string(i) + "_" + std::to_string(j);
            char *yname = (char *) (&name[0]);
            int ccnt = 1;
            CHECKED_CPX_CALL(CPXnewcols, env, lp, ccnt, &distance_matrix[i][j], &lb, &ub, &ytype, &yname);

            ys.push_back({current_var_position, i, j});
            map_y[i][j] = current_var_position;
            current_var_position++;
        }
    }

    for (int i = 0; i < N; i++) {
        for (int j = 1; j < N; j++) {
            char xtype = 'C';
            double lb = 0.0;
            double ub = CPX_INFBOUND;
            std::string name = "x_" + std::to_string(i) + "_" + std::to_string(j);
            char *xname = (char *) (&name[0]);
            int ccnt = 1;
            double cost = 0;
            CHECKED_CPX_CALL(CPXnewcols, env, lp, ccnt, &cost, &lb, &ub, &xtype, &xname);

            xs.push_back({current_var_position, i, j});
            map_x[i][j] = current_var_position;
            current_var_position++;
        }
    }

    // ------------------------------------------------------------
    // --------------- CONSTRAINTS
    // ------------------------------------------------------------
    // add (10) [in o.f.: sum{i: (i,k) in A} x_ik - sum{j: (k,j), j!=0} x_kj = 1 for all k in N\{0}]
    for (int k = 1; k < N; k++) {
        vector<int> id_x;
        vector<double> coef;
        char sense = 'E';
        int matbeg = 0;
        int colcnt = 0;
        int rowcnt = 1;
        double rhs = 1;

        for (int i = 0; i < N; i++) {
            if (i != k) {
                id_x.push_back(map_x[i][k]);
                coef.push_back(1.0);
            } else {
                continue;
            }
        }

        for (int j = 1; j < N; j++) {
            if (j != k) {
                id_x.push_back(map_x[k][j]);
                coef.push_back(-1.0);
            } else {
                continue;
            }
        }
        CHECKED_CPX_CALL(CPXaddrows, env, lp, colcnt, rowcnt, id_x.size(), &rhs, &sense, &matbeg, &id_x[0], &coef[0],
                         NULL, NULL);
    }


    // add (11) [in o.f.: sum{j:(i,j) in A} y_ij = 1 for all i]
    for (int i = 0; i < N; i++) {
        std::vector<int> id_y(N);
        std::vector<double> coef(N, 1.0);
        char sense = 'E';
        for (int j = 0; j < N; j++) {
            id_y[j] = map_y[i][j];
        }
        int matbeg = 0;
        int colcnt = 0;
        int rowcnt = 1;
        double rhs = 1;
        CHECKED_CPX_CALL(CPXaddrows, env, lp, colcnt, rowcnt, id_y.size(), &rhs, &sense, &matbeg, &id_y[0], &coef[0],
                         NULL, NULL);
    }

    // add (12) [in o.f.: sum{i:(i,j) in A} y_ij = 1 for all j]
    for (int j = 0; j < N; j++) {
        std::vector<int> id_y(N);
        std::vector<double> coef(N, 1.0);
        char sense = 'E';
        for (int i = 0; i < N; i++) {
            id_y[i] = map_y[i][j];
        }
        int matbeg = 0;
        int colcnt = 0;
        int rowcnt = 1;
        double rhs = 1;
        CHECKED_CPX_CALL(CPXaddrows, env, lp, colcnt, rowcnt, id_y.size(), &rhs, &sense, &matbeg, &id_y[0], &coef[0],
                         NULL, NULL);
    }


    // add (13) [x_ij + (1-N)*y_ij <= 0 for all i,j]
    for (int i = 0; i < N; i++) {
        for (int j = 1; j < N; j++) {
            std::vector<int> id_x(2);
            std::vector<double> coef(2);
            char sense = 'L';
            id_x[0] = map_x[i][j];
            id_x[1] = map_y[i][j];
            coef[0] = 1.0;
            coef[1] = (1 - N); // 1-N == -(N-1)
            int matbeg = 0;
            double rhs = 0;
            CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, id_x.size(), &rhs, &sense, &matbeg, &id_x[0], &coef[0], NULL,
                             NULL);
        }
    }
    CPXchgobjsen(env, lp, CPX_MIN);
    std::string lp_file = std::to_string(N) + "-" + std::to_string(lpcount) + "-" + timestamp +
                          ".lp";
    CHECKED_CPX_CALL(CPXwriteprob, env, lp, lp_file.c_str(), 0);
}


int main(int argc, char const *argv[]) {
  try {

    FileManager f = FileManager();
    std::string data_folder_path= "data";

    // -- CONFIGURATION ---
    //Configure the instance you want to solve here ("data/10/distance-matrix-10-4.txt" is the path of the 4-th instance with 10 nodes)
    int nodes_amount = 10;   //values accepted: 10, 25, 50, 100, 150
    int instance_number = 0; //value accepted: 0,1,2,3,4
    // --------------------

    //Path set up
    const std::string file_name_ext = "distance-matrix-" + std::to_string(nodes_amount) + "-" + std::to_string(instance_number) + ".txt";
    const std::string file_name = "distance-matrix-" + std::to_string(nodes_amount) + "-" + std::to_string(instance_number);
    const std::string file_to_read = data_folder_path  + "/" + std::to_string(nodes_amount)  + "/" + file_name_ext;

    //init env and lp
    DECL_ENV(env);
    DECL_PROB(env, lp);

    // read distance matrix from the selected file
    vector<vector<double> > distance_matrix = f.read(file_to_read, "distance");

    // setup lp
    int num_vars;
    std::vector<arc> ys;
    std::vector<arc> xs;
    std::string now = current_timestamp();
    setupLP(env, lp, num_vars, distance_matrix, ys, xs, now, 0);

    // start the timer and run optimization 
    std::cout << "Solving the following instance: " << file_to_read << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    CHECKED_CPX_CALL(CPXmipopt, env, lp);

    // stop timer and get elapsed time
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    // get object function value
    double objval;
    CHECKED_CPX_CALL(CPXgetobjval, env, lp, &objval);
    int n = CPXgetnumcols(env, lp);

    //retrieve variables
    std::vector<double> var_vals;
    var_vals.resize(n);
    CHECKED_CPX_CALL(CPXgetx, env, lp, &var_vals[0], 0, n - 1);

    // retrieve y variables and build the arcs of the solution
    std::vector<arc> arcs = retrieve_arcs_from_solution(var_vals, ys);

    // retrieve the best hamiltonian path found by the solver
    std::string best_hamiltonian_path = retrieve_cycle(arcs);

    // write solution to file ".sol"
    std::string file_name_no_ext = remove_txt_extension(file_name);
    std::string sol_file = "solution-" + file_name_no_ext + "-" + now +
                            ".sol";
    CHECKED_CPX_CALL(CPXsolwrite, env, lp, sol_file.c_str());

    // free memory
    CPXfreeprob(env, &lp);
    CPXcloseCPLEX(&env);

    //output solution info
    std::cout << "-------------------------------------------------------------";
    std::cout << "Instance: " << file_name << std::endl;
    std::cout << "Number of nodes:" << nodes_amount << std::endl;
    std::cout << "Time spent:" << std::to_string(elapsed.count()) << std::endl;
    std::cout << "Objective function value:" << std::to_string(elapsed.count()) << std::endl;
    //cycle path
    std::cout << "Cycle: [" << best_hamiltonian_path << "]" << std::endl;
    std::cout << "-------------------------------------------------------------";
            
    
  } catch (std::exception &e) {
    std::cout << ">>>EXCEPTION: " << e.what() << std::endl;
  }
  return 0;
}
