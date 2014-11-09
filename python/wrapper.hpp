// A helper class to interface Python Front-End to C++ Kodiak library.
// Only contains a Wrapper class which takes care of all the functionality.
// It is used in the interface.pyx Interface class.
// With additional boiler plate code, could be used on its own.

#include <kodiak.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace kodiak;
typedef unsigned int nat;
typedef double real;

class Wrapper {
public:
  Wrapper();
  ~Wrapper();
  void set_safe_input(const bool val)  {Kodiak::set_safe_input(val);};
  bool is_safe_input() {return Kodiak::safe_input();};

  // Sets the variable prefix in Kodiak.
  void set_var_prefix(const std::string prefix = K_PREFIX);

  // Creates an kodiak::Interval from a double bypassing checks for floating point values
  // Necessary when entering hexadecimal floating point constants.
  Interval from_hex(const real n);

  // Builds polynomials by applying op on Real arguments.
  Real build_poly(const std::string op, const Real r, const Real r2);
  Real build_poly(const std::string op, const Real r);

  //Helper function to print the value of real from Python interface
  template <class T>
  void print_c(T n) {std::cout << n << std::endl;};

  // Assigns the parameters to problems and solves them.
  // Sets this->pav_ans to hold the produced paving.
  void pave();
  void bifurcation(bool equilibrium = false);
  void minmax(std::string mode = "minmax");

  void add_var(std::string name, Interval i);

  void add_param(std::string name, Interval i);

  void add_dfeq(Real dfeq);

  void add_constraint(Real c, std::string op);

  void add_const(std::string name, Interval val);

  void add_def(std::string name, Real val);

  void set_objfn(Real p) {objfn = p;};

  void set_precision(int n) {precision = n;};

  void set_resolution(real n) {resolution = n;};

  void set_resolution(std::string name, real n);

  void set_granularity(real n) {granularity = n;};

  void set_paving_mode(SearchType m) {paving_mode = m;};

  void set_bp(bool v) {bp = v;};

  // Set the name for the entire problem.
  void set_name(std::string n) {name = n;};

  void set_depth(nat n) {depth = n;};

  void set_var_select(int n) {var_select = n;};

  void set_debug(bool n) {Kodiak::set_debug(n);};

  // Sets the file to which all the output will be redirected.
  void set_out_file(std::string n) {out_file_name = n;};

  std::string get_out_file() {return out_file_name;}

  bool file_exists(const char *fileName) {
    std::ifstream infile(fileName);
    return infile.good();
  };


  void save_paving(std::string file_name) {pav_ans.write(file_name);}
  void load_paving(std::string file_name) {pav_ans.read(file_name);}

  void plot2D(std::string name, std::string var1, std::string var2);
  void plot3D(std::string name, std::string var1, std::string var2, std::string var3);
  std::vector<Boxes*> get_boxes();

  //
  // In the following templated functions prob must be an instance of a class extending kodiak::System.
  //

  // Assigns variables stored in var_names and var_intervals to prob.
  template<class T>
  void assign_vars(T &prob);

  // Assigns parameters stored in param_names and param_intervals to prob.
  template<class T>
  void assign_params(T &prob);

  // Assings const stored in const_names and const_vals to prob.
  template<class T>
  void assign_consts(T &prob);

  // Assigns definition stored in defs to problem.
  template<class T>
  void assign_defs(T &prob);

  // Assigns differential expressions stored in dfeqs to prob.
  template<class T>
  void assign_dfeqs(T &prob);

  // Assigns constraints stored in constraints to prob.
  template<class T>
  void assign_constraints(T &prob);

  // Assigns resolutions stored in resolutions to prob.
  template<class T>
  void assign_resolutions(T &prob);

  // Calls the print method of the problem.
  // If out_file set, contents are printed to that file.
  template<class T>
  void print(T &prob);

private:

  Names var_names;
  Names param_names;
  Names const_names;

  Box var_intervals;
  Box param_intervals;

  std::vector<Interval> const_vals;
  std::vector<Real> constraints;
  std::vector<std::string> operators;
  std::vector<Real> dfeqs;
  std::vector<std::pair<std::string, real> > resolutions;
  std::vector<std::pair<std::string, Real> > defs;

  // Paving to the last solved problem is held here.
  Paving pav_ans;

  int precision;
  real resolution;
  real granularity;
  nat depth;
  int var_select;
  SearchType paving_mode;
  bool equilibrium;
  bool bp;
  std::string name;
  Real objfn;
  // When true print method does not print anything.
  bool no_outputs;

  std::string out_file_name;
};
