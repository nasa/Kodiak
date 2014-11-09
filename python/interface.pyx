# Cython interface for connecting Python Front-End to Kodiak C++ library.
#
# Uses helper C++ class Wrapper.
#
from __future__ import print_function
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.pair cimport pair
from numbers import Number
from cython.operator cimport dereference as deref
import sys

# type definitions for compatibility to kodiak
ctypedef unsigned int nat
ctypedef bint bool
ctypedef double real
ctypedef vector[Interval] Box 
ctypedef vector[string] Names
ctypedef vector[pair[string, Real]] Defs
ctypedef vector[Box] Boxes

cdef extern from "kodiak.hpp" namespace "kodiak::Kodiak":
	bool debug()

cdef extern from "kodiak.hpp" namespace "kodiak":
# Importing global functions from kodiak namespace
	Real var(const nat v, const string name)
	Real var(const string name)
	Real val(const Interval i)
	Real cnst(const string name)
	Real let(const string name, const Real, const Real)

	Interval ival(const Interval, const Interval)
	Interval ival(const real, const real)
	Interval approx(real n)
	Interval rat(const int num, const nat denum)
	Interval dec(const int num, const int exp)
	Interval hex_val(const real val) 
	cdef enum SearchType:
		STD
		FIRST
		FULL
# Importing all the used classes from Kodiak  
	cdef cppclass Real:
		Real (const Real &)
		Real ()

	cdef cppclass Interval:
		real inf()
		real sup()

# Importing Wrapper class
# All the functions and values which you want to use in Interface from Wrapper class have to be defined here.	
cdef extern from "wrapper.hpp":
	cdef cppclass Wrapper:
		Wrapper()
		void set_safe_input(const bool val)
		bool is_safe_input()
		
		void add_var(string name, Interval i)
		void add_param(string name, Interval i)
		void add_const(string name, Interval val)
		void add_constraint(Real c, string op)
		void add_dfeq(Real dfeq)
		void add_def(string name, Real val)
		
		void set_objfn(Real p)
		void set_precision(int n)
		void set_granularity(real n)
		void set_resolution(real n)
		void set_resolution(string name, real n)
		void set_paving_mode(SearchType m)
		void set_bp(bool n)
		void set_name(string n)
		void set_depth(nat n)
		void set_var_select(int n)
		void set_debug(bool val)
		void set_out_file(string name)

		string get_out_file()
		vector[Boxes*] get_boxes()
				
		void save_paving(string file_name)
		void load_paving(string file_name)
		void plot2D(string name, string var1, string var2)
		void plot3D(string name, string var1, string var2, string var3)

		void print_c[T](T v)
		Real build_poly(const string op, const Real r, const Real r2)
		Real build_poly(const string op, const Real r)
		void pave() except +RuntimeError
		void bifurcation(bool equilibrium) except +RuntimeError
		void minmax(string mode) except +RuntimeError
		
# Class which interfaces Python code to C++ code		
cdef class Interface:
	'''Class which interfaces Python Front-End code to C++ Kodiak Library

		All the interaction from Python code to C++ code go through this middle class.
		It serves commands from parser directly creating C++ objects for the Kodiak Library.

		It is intended to potentially be hierarchical structure of interfaces, but as in current implementation
		one interface is enough, to simplify code it is kept like this.

		Also should eventually be converted to be used in 'with' construct
	''' 
	# Holds pointer to the instance of Wrapper class
	cdef Wrapper* _wrapper

	# Holds the names of cnsts to prevent creating additional copies
	cdef int _const_n
	_const_names = {}
	# List holding the names of named nodes, to reduce duplications
	_nn_names = []
	# List holding all the name nodes, used for caching them
	cdef vector[Real] _name_nodes

	def __init__(self):
		self._wrapper = new Wrapper()
		self._const_n = 0

	def __enter__(self):
		return self

	def __exit__(self, type, value, traceback):
		del self._wrapper
	
	# Setters  and getters for values in Kodiak and Wrapper
	def set_safe_input(self, n):
		self._wrapper.set_safe_input(self.to_bool(n))

	def is_safe_input(self):
		return self._wrapper.is_safe_input()

	def set_precision(self, n):
		self._wrapper.set_precision(n)
	
	def set_resolution(self, n):
		self._wrapper.set_resolution(n)
	
	def set_resolution_var(self, name, n):
		'''Sets resolution for particular variable

		   Args:
		   		name: name of the variable for which resolution will be set
		   		n: double containing the resoulution	
		'''
		self._wrapper.set_resolution(name, n)	

	def set_granularity(self, n):
		self._wrapper.set_granularity(n)	

	def set_paving_mode(self, mode):
		if(mode.lower() == "std"):
			self._wrapper.set_paving_mode(STD)
		elif(mode.lower() == "first"):
			self._wrapper.set_paving_mode(FIRST)
		else:
			self._wrapper.set_paving_mode(FULL)
	
	def set_bp(self, val):
		self._wrapper.set_bp(self.to_bool(val))
	
	def set_name(self, name):
		'''Sets the name of problem'''
		self._wrapper.set_name(name)
		
	def set_depth(self, val):
		self._wrapper.set_depth(val)

	def set_var_select(self, val):
		self._wrapper.set_var_select(val)

	def set_debug(self, val):
		'''Sets the debbuging flag for Kodiak, Wrapper and Interface'''
		self._wrapper.set_debug(self.to_bool(val))

	def set_out_file(self, name):
		self._wrapper.set_out_file(name)


	def get_out_file(self):
		return self._wrapper.get_out_file()
	
	def reset_env(self):
		'''Completely resets the working environment, so that a new problem could be loaded

			This is not done automatically to allow testing the same problem with additional variables and etc.
		'''
		print ("Resetting...")
		self._const_names.clear()
		self._const_n = 0
		self._nn_names[:] = []
		self._name_nodes.clear()
		del self._wrapper 
		self._wrapper = new Wrapper()
		print ("Done!")

	# helper funciton to convert input to legal boolean value
	# Cython and C++ boolean values are converted automatically between each other
	cdef bool to_bool(self, val):
		'''Helper funtion to convert values to from Parser to boolean values

		   Args:
		   		val: 'true' or 'false' string.

		   Returns:
		   		Either True or false depending on the input.
		'''
		if(val.lower() == 'true'):
			return True
		else:
			return False
		
	#recursively evaluates numerical expression from parser	
	cdef  Real evaluate_numexp(self, exp):
		'''Evaluates an AST of numexp passed from Parser

		   Args:
		   		exp: Nested tuple containing AST of numexp

		   Returns:
		   		A type Real value ready to be used in Kodiak Library.
		'''
		cdef Real exp1
		cdef Real exp2
		#if exp is not a collection type just convert it to real
		if(self.is_number(exp) or type(exp) is str):
			return self.to_Real(exp)

		#if exp is a tuple, that means that the second part is a numerical expression
		#and the first one is an operator
		if(len(exp) == 2):
			if(type(exp[1]) == tuple):
				exp1 = self.evaluate_numexp(exp[1])
			else:
				if(exp[0] == "approx"):
					exp1 = val(approx(exp[1]))
				else:
					exp1 = self.to_Real(exp[1])
			return self._wrapper.build_poly(exp[0].lower(), exp1)

		#if exp is a tuple, that means, that the first part is an operator
		#and two remaining parts are numerical expressions
		if(len(exp) == 3):
			#if the any of the expressions in a binary expression are complex evaluate them
			#else just convert them to Real values for easier building of polynomial
			if(type(exp[1])==tuple):
				exp1  = self.evaluate_numexp(exp[1])
			else:
				exp1 = self.to_Real(exp[1])
			if(type(exp[2])==tuple):
				exp2 = self.evaluate_numexp(exp[2])
			else:
				exp2 = self.to_Real(exp[2])
			return self._wrapper.build_poly(exp[0], exp1, exp2) 

	cdef Real to_Real(self, exp):
		'''Converts a Python object to kodiak::Real object for use in evaluate_numexp

			Args:
				exp: A Python object which should be converted to kodiak::Real.
					 It can be a number, a hexadecimal number or a string containing name of Node in Kodiak

			Returns:
					 A real value of appropriate type.
		'''
		# Cecks if exp is an int
		if(type(exp) is int):
			return val(ival(<real>exp, <real>exp))
		# Checks if exp is any other type of number
		# i.e. long, float, complex
		elif(self.is_number(exp)):
			return val(ival(<real>exp, <real>exp))
		elif(type(exp) == str):
			# Checks if exp is a hexadecimal constant
			if(exp[0] == '0'):
				return val(hex_val(float.fromhex(exp)))
			#if none of before it must be a variable 
			else:
				# If name is defined to be a constant, creates a constant.
				if(exp in self._const_names):
					return self.create_const(exp)
				# Else it is a name node.
				else:
					return self.create_Name_Node(exp)

	
	cdef Interval evaluate_intervals(self, exp):
		'''Converts a Python object to kodiak::Interval

			Args: Either a Tuple containing type of expression and values
				  i.e. ("rat", 5, 10), ("approx", 2.2) or an integer

			Returns: A value of kodiak::Interval with the correct type, i.e. a rational or decimal.
		'''
		cdef Interval i, i2
		cdef real r, r2
		cdef int n, n1
		if(self.is_number(exp) or exp[0] == 'u-'):
			r = exp
			return ival(<real>r, <real>r)
		if(type(exp) is str):
			i = (hex_val(float.fromhex(exp)))
			return i
		r = exp[1] 
		if(len(exp) == 2):
			if(exp[0] == "approx"):
				if(debug()):
					print("approximating ", r)
				i = approx(r)
				return i
		if(len(exp) == 3):
			r2 = exp[2]
			if(exp[0] == "dec"):
				return dec(<int>r, <int>r2)
			if(exp[0] == "rat"):
				return rat(<int>r, <nat>exp[2])

	cdef bool is_number(self, n):
		'''A convenience function to check if n is numerical type'''
		return isinstance(n, Number)
	
	def objfn_exp(self, exp):
		'''Evaluates an AST of Polynomial passed from Parser and stores it in Wrapper.

		   The evaluated polynomial is stored in Wrapper object objfn field for use in MinMax problems.

		   Callled from Parser.

		   Args:
		   		exp: An AST encoded as a Tuple from Parser.
		'''
		cdef Real p = self.evaluate_numexp(exp)
		self._wrapper.set_objfn(p)
		
	def dfeq_exp(self, exp):
		'''Evaluates an AST of Differential Expression passed from Parser and stores it in Wrapper.

		   The evaluated polynomial is stored in Wrapper object dfeqs vector for use in Bifurcation problems.

		   Callled from Parser.

		   Args:
		   		exp: An AST encoded as a Tuple from Parser.
		'''
		cdef Real p = self.evaluate_numexp(exp)
		self._wrapper.add_dfeq(p)

	def constraint_exp(self, exp):
		'''Evaluates an AST of a Constraint passed from Parser and stores it in Wrapper.

		   The evaluated polynomial is stored in Wrapper object for use in all types of problems.

		   Callled from Parser.

		   Args:
		   		exp: An AST encoded as a Tuple from Parser.
		'''
		cdef Real c = self.evaluate_numexp(exp[1])
		self._wrapper.add_constraint(c, exp[0])

	# creates a new var or param and adds it to _wrapper object
	# if var true, creates a new var
	# if false creates a new param
	def var_exp(self, name, interval, bool var = True):
		'''Creates a new variable or parameters for Kodiak and stores it in Wrapper

		   To create a new variable this method should be called with 2 parameters.
		   To create a new param a third parameter should be set to False.

		   If a problem is detected with in the input, new variable is not created.

		   Callled from Parser.

		   Args:
		       name: Name of the new variables.
		       interval: A tuple of format ('INTERVAL', lb, ub).
		       var: A switch to determine if new variable or parameter should be created
		'''
		cdef Interval i,i1,i2
		if(name in self._nn_names):
			print("ERROR: " + name + " is already defined. Input ignored", file=sys.stderr)
			return
		i1 = self.evaluate_intervals(interval[1])
		i2 = self.evaluate_intervals(interval[2])
		if(i2.inf() < i1.inf()):
			print("ERROR: Higher bound lower than lowerbound. Input ignored.\n", file=sys.stderr)
			return
		i = ival(i1, i2)
		self.create_Name_Node(name)
		if(var):
			self._wrapper.add_var(name, i)
		else:
			self._wrapper.add_param(name, i)

	def const_exp(self, name, val):
		'''Adds a constant to Wrapper

		   Callled from Parser.

		   Args:
				name: name of the constant
				val: value for the new constant
		'''
		cdef Interval i
		i = self.evaluate_intervals(val)
		self.create_const(name)
		self._wrapper.add_const(name, i)

	cdef Real evaluate_let(self, decl, type_of_exp ,exp):
		'''Evaluates simple and nested let expressions.

			Args:
				decl: Declaration part of let exp. i.e. x = y^5 or x = y^2, z = x*2-5.
				type_of_exp: Type of expression for the let exp.
							 Possible values: 'cnstr', 'objfn' and 'dfeq'
				exp: Expression for which the declaration part applies.

			Return:
				kodiak::Real with appropriate simple or nested let expression.
		'''
		cdef Real lexp, iexp, val
		# if length of decl(arations) tuple is 2, expression not nested,
		# just evaluate the second part and return a Real let value
		if(len(decl)==2):
			# create a new name node, so that the name in decl is known to evaluator
			self.create_Name_Node(decl[0])
			# evaluate let part of the expression
			lexp = self.evaluate_numexp(decl[1])
			# check the type of the expression, if constraint, that means that the first part
			# of exp is an operator, so it has to be evaluated differently
			if(type_of_exp.lower() == "cnstr"):
				iexp = self.evaluate_numexp(exp[1])
				val = let(decl[0], lexp, iexp)
			else:
				iexp = self.evaluate_numexp(exp)
				val = let(decl[0], lexp, iexp)
			return val
		# if length of decl(arations) tuple is 3, expression nested,
		# evaluate the first two parts and recursively evaluate the third
		if(len(decl) == 3):
			self.create_Name_Node(decl[0])
			lexp = self.evaluate_numexp(decl[1])
			#recursively evaluate the nested let value
			iexp = self.evaluate_let(decl[2], type_of_exp, exp)
			val = let(decl[0], lexp, iexp)
			return val

	def let_exp(self, type_of_exp, decl, exp):
		'''Evaluates let expressions and stores them in Wrapper object.

			Callled from Parser.

			Args:
				decl: Declaration part of let exp. i.e. x = y^5 or x = y^2, z = x*2-5.
				type_of_exp: Type of expression for the let exp.
							 Possible values: 'cnstr', 'objfn' and 'dfeq'
				exp: Expression for which the declaration part applies.
		'''
		cdef Real let = self.evaluate_let(decl, type_of_exp,exp)
		if(type_of_exp.lower() == 'cnstr'):
			self._wrapper.add_constraint(let, exp[0])
		elif(type_of_exp.lower() == 'objfn'):
			self._wrapper.set_objfn(let)
		elif(type_of_exp.lower() == 'dfeq'):
			self._wrapper.add_dfeq(let)


	def def_exp(self, name, exp):
		'''Evaluates define expression and stores them in Wrapper object

			Called form Parser.

			Args:
				name: name of the global definition
				exp: value of the global definition

		'''
		cdef Real r
		self.create_Name_Node(name)
		r = self.evaluate_numexp(exp)
		self._wrapper.add_def(name, r)
		
	cdef Real create_Name_Node(self, name):
		'''Creates a kodiak::Name_Node for use in building polynomials.

			If the name node with this name is requested the first time it is created.
			Else a previously created one form the list is returned.

			Args:
				name: name of the node.

			Returns:
				A name node with the correct name.

		'''
		cdef Real nn
		if(not name in self._nn_names):
			nn = var(name)
			self._nn_names.append(name)
			self._name_nodes.push_back(nn)
			return nn
		else:
			n = self._nn_names.index(name)
			return self._name_nodes[n]
	
	cdef Real create_const(self, name):
		'''Creates a Constant Node for use in building polynomials.

			Also adds all constant names to the self._const_names to check if a new name node should be a constant 

			Args:
				name: name of the node.

			Returns:
				A name node with the correct name.

		'''
		if(self._const_names.has_key(name)):
			r = cnst(name)
		else:
			r = cnst(name)
			self._const_names[name] = self._const_n
			self._const_n = self._const_n + 1
		return r


	def pave(self):
		'''Tells the Wrapper to issue Pave command to Kodiak library.

			Called from Parser.
		'''
		try:
			self._wrapper.pave()
		except RuntimeError as e:
			raise KodiakError(e.message)

	def bifurcation(self, equilibrium = False):
		'''Tells the Wrapper to issue Bifurcation command to Kodiak library.

		   Called from Parser.

		   Args:
		   		equilibrium: optional argument, if True, an Equilibrium problem is solved.
		'''
		try:
			self._wrapper.bifurcation(equilibrium)
		except RuntimeError as e:
			raise KodiakError(e.message)
		
	def minmax(self, mode = "minmax"):
		'''Tells the Wrapper to issue MinMax command to Kodiak library.

		   Depending on the mode a different MinMax problem is solved.

		   Called from Parser.

		   Args:
		   		mode: Optional argument, can be 'min', 'max' or 'minmax'
		   			  Default value 'minmax'
		'''
		try:
			self._wrapper.minmax(mode)
		except RuntimeError as e:
			raise KodiakError(e.message)

	def save_paving(self, file_name):
		'''Saves the current paving to a file 'file_name'

			Args:
				file_name: name of the file in which the paving will be saved.
		'''
		self._wrapper.save_paving(file_name)

	def load_paving(self, file_name):
		'''Loads a paving from a file

		   Args:
		   		file_name: name of the file from which the paving should be loaded.
		'''
		self._wrapper.load_paving(file_name)
		print("Paving found in " + file_name +".pav loaded")

	def plot(self, name, vars):
		'''Issues plot command to Wrapper.

			Args:
				name: Name of the plot file.
				vars: A list of variable names for which the plot will be drawn.
		'''
		if(len(vars) == 2):
			self._wrapper.plot2D(name, vars[0], vars[1])
		else:
			self._wrapper.plot3D(name, vars[0], vars[1], vars[2])

	def prepareboxes(self):
		cdef vector[Boxes*] bob = self._wrapper.get_boxes()
		conv_boxes = []
		for i in xrange(bob.size()):
			temp_boxes = []
			for o in deref(bob[i]):
				box = []
				for p in o:
					box.append([p.inf(), p.sup()])
				temp_boxes.append(box)
			conv_boxes.append(temp_boxes)
		return conv_boxes

class KodiakError(Exception):
	'''An exception class to signify that an exception was produced by Kodiak'''
	def __init__(self, value):
		self.value = value

	def __str__(self):
		return repr(self.value)

