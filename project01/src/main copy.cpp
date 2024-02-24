#include "cpxmacro.h"
#include "FileManager.h"
#include <cstdio>
#include <iostream>
#include <vector>
using namespace std;

struct arc {
    int var_position;
    int start;
    int end;
};

char** stringToCharPtrPtr(const std::string& str) {
    char* cstr = new char[str.length() + 1];
    std::strcpy(cstr, str.c_str());

    char** cstr_ptr = new char*[1];
    cstr_ptr[0] = cstr;

    return cstr_ptr;
}


// error status and messagge buffer
int status;
char errmsg[BUF_SIZE];

// starting node index
const int INIT = 0;

void setupLP(CEnv env, Prob lp, int &current_var_position, const vector<vector<double>> &distance_matrix, std::vector<arc>& ys, std::vector<arc>& xs) {

  current_var_position = 0;
  const int N = distance_matrix.size();

  // ------------------------------------------------------------
  // --------------- MAPS
  // ------------------------------------------------------------

  // map_x[i][j] = x_ij
	vector<vector<int> > map_x;	
  map_x.resize(N);
	for ( int i = 0 ; i < N ; ++i ) {
		map_x[i].resize(N);
		for ( int j = 0 ; j < N ; ++j ) {
			map_x[i][j] = -1;
		}
	} 
  // map_y[i][j] = y_ij
  vector<vector<int> > map_y;
  map_y.resize(N);
	for ( int i = 0 ; i < N ; ++i ) {
		map_y[i].resize(N);
		for ( int j = 0 ; j < N ; ++j ) {
			map_x[i][j] = -1;
		}
	} 

  // ------------------------------------------------------------
  // --------------- VARIABLES 
  // ------------------------------------------------------------

  // add y vars [in o.f.: sum{i,j} C_ij y_ij]
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      char ytype = 'B';
      double lb = 0.0;
      double ub = 1.0;
      std::string name = "y_" + std::to_string(i) + "_" + std::to_string(j);
      char *yname = (char *)(&name[0]);
      int ccnt = 1;
      CHECKED_CPX_CALL(CPXnewcols, env, lp, ccnt, &distance_matrix[i][j], &lb, &ub, &ytype, &yname);

      ys.push_back({current_var_position, i, j});
    	map_y[i][j] = current_var_position;
      current_var_position++;
    }
  }

  // add x vars
  for (int i = 0; i < N; i++) {
    for (int j = 1; j < N; j++) {
      char xtype = 'C';
      double lb = 0.0;
      double ub = CPX_INFBOUND;
      std::string name = "x_" + std::to_string(i) + "_" +  std::to_string(j);
      char *xname = (char *)(&name[0]);
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
  for (int k=1; k<N; k++){
    vector<int> id_x;
    vector<double> coef;
    char sense = 'E';

    //TODO aggiungere continue per ott. creazione variabili
    
    for (int i=0; i<N; i++){
        if(i != k) {
          id_x.push_back(map_x[i][k]);
          coef.push_back(1.0);
        } else{
          continue;
        }
    }

    for (int j=1; j<N; j++){
      if (j != k ){
        id_x.push_back(map_x[k][j]);
        coef.push_back(-1.0);
      } else {
        continue;
      }
    }

    int matbeg = 0;
    int colcnt = 0;
    int rowcnt = 1;
    double rhs = 1;

		CHECKED_CPX_CALL(CPXaddrows, env, lp, colcnt, rowcnt, id_x.size(), &rhs, &sense, &matbeg, &id_x[0], &coef[0], NULL      , NULL      );
 
  }


	// add (11) [in o.f.: sum{j:(i,j) in A} y_ij = 1 for all i]
	for (int i = 0; i < N; i++)
	{
		std::vector<int> id_y(N);
		std::vector<double> coef(N, 1.0);
		char sense = 'E';
		for (int j = 0; j < N; j++)
		{
			id_y[j] = map_y[i][j];
		}
		int matbeg = 0;
    int colcnt = 0;
    int rowcnt = 1;
    double rhs = 1;
  	CHECKED_CPX_CALL(CPXaddrows, env, lp, colcnt, rowcnt, id_y.size(), &rhs, &sense, &matbeg, &id_y[0], &coef[0], NULL      , NULL      );

	}

	// add (12) [in o.f.: sum{i:(i,j) in A} y_ij = 1 for all j]
	for (int j = 0; j < N; j++)
	{
		std::vector<int> id_y(N);             // create a vector of size N of indexes of y_ij
		std::vector<double> coef(N, 1.0);     // create a vector of size N of coefficients for y_ij
		char sense = 'E';
		for (int i = 0; i < N; i++)
		{
			id_y[i] = map_y[i][j];
		}
		int matbeg = 0;
    int colcnt = 0;
    int rowcnt = 1;
    double rhs = 1;  
    CHECKED_CPX_CALL(CPXaddrows, env, lp, colcnt, rowcnt, id_y.size(), &rhs, &sense, &matbeg, &id_y[0], &coef[0], NULL      , NULL      );

	}


  // add (13) [x_ij - (N-1)y_ij <= 0 for all i,j]
  for (int i = 0; i < N; i++){
    for (int j = 0; j < N; j++){

      if(j != INIT){
        std::vector<int> id_x(2);     // create a vector of size 2 of indexes of x_ij and y_ij
        std::vector<double> coef(2);  // create a vector of size 2 of coefficients for x_ij and y_ij
        char sense = 'L';
        id_x[0] = map_x[i][j];
        id_x[1] = map_y[i][j];
        coef[0] = 1.0;
        coef[1] = -(N-1);
        int matbeg = 0;
        double rhs = 0;
        CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, id_x.size(), &rhs, &sense, &matbeg, &id_x[0], &coef[0], NULL, NULL);
      }
    }
  }


  CHECKED_CPX_CALL(CPXwriteprob, env, lp, "lab01.lp", 0);

}

int main(int argc, char const *argv[]) {
  try {
    // init
    DECL_ENV(env);
    DECL_PROB(env, lp);
    // setup LP
    int num_vars;
    std::vector<arc> ys;
    std::vector<arc> xs;
    
    FileManager f = FileManager();
    vector <vector<double>> distance_matrix = f.read("/home/1/2018/lperinel/Desktop/memoco/project01/data/500/distance-matrix-500-0.txt", "distance");

    /** uncomment to print distance matrix (not recommended for large matrices)
    for (int i=0; i<distance_matrix.size(); i++){
      for (int j=0; j<distance_matrix.size(); j++){
        std::cout << distance_matrix[i][j] << " ";
      }
      std::cout << std::endl;
    }
    */

    setupLP(env, lp, num_vars, distance_matrix, ys, xs);
    // optimize
    CHECKED_CPX_CALL(CPXmipopt, env, lp);
    // print
    double objval;
    CHECKED_CPX_CALL(CPXgetobjval, env, lp, &objval);
    std::cout << "Objval: " << objval << std::endl;
    int n = CPXgetnumcols(env, lp);
    std::vector<double> varVals;
    varVals.resize(n);
    CHECKED_CPX_CALL(CPXgetx, env, lp, &varVals[0], 0, n - 1);
    for (int i = 0; i < ys.size(); ++i) {

      int pos = ys[i].var_position;
      int start = ys[i].start;
      int end = ys[i].end;

      if(varVals[pos] >= 1) {
        std::cout << "arc from " << start << " to " << end  << std::endl;
      }
    }
    CHECKED_CPX_CALL(CPXsolwrite, env, lp, "lab01.sol");
    // free
    CPXfreeprob(env, &lp);
    CPXcloseCPLEX(&env);
  } catch (std::exception &e) {
    std::cout << ">>>EXCEPTION: " << e.what() << std::endl;
  }
  return 0;
}
