#---------------------------------------------------------
# Parser for Kodiak
# uses PLY libraries
#
# raises SyntexError on incorrect input
# accepts empty input
#
# author: Mantas
#---------------------------------------------------------

#################### Grammar ##################################
# 
# TERMINALS in all capital
#
# nonterminals in lowercase
#
# statement ::= expression
# 
# expression ::= expression ENDSTATEMENT expression | expression ENDSTATEMENT
#    
# expression ::= VAR ID IN interval | PARAM ID IN interval | CONST ID EQUALS number
#
# expression ::= CNSTR boolexp | DFEQ numexpression | OBJFN numexpression
#
# number ::= NUMBER | UMINUS NUMBER | HEX | APRROX LPAREN number RPAREN | DEC LPAREN number COMMA number RPAREN | RAT number COMMA NUMBER RPAREN
#                
# numexpression ::= number| ID | numexpression !binary_math_op! numexpression
#                   UMINUS numexpression | LPAREN numexpression RPAREN 
#
# boolexp :: = numexpression cmp_op numexpression
#
# interval ::= LBRACKET numexpression COMMA numexpression RBRACKET
#
# expression ::= SET PRECISION EQUALS NUMBER | SET RESOLUTION EQUALS NUMBER | SET RESOLUTION ID EQUALS NUMBER | SET SAFE INPUT EQUALS bool | SET DEBUG = bool
#
# expression ::= SET DEPTH EQUALS NUMBER | SET VARSELECT EQUALS NUMBER | SET SAFE INPUT EQUALS bool | SET NAME = ID | SET BP = bool
#
# expression ::= SET PAVING MODE EQUALS STD | SET PAVING MODE EQUALS FIRST | SET PAVING MODE EQUALS FULL
#
# expression ::= PAVE | BIFURCATION | MINMAX | MIN | MAX
#
# expression ::= OBJFN LET let_exp IN numexpression | DFEQ LET let_exp IN numexpression | CNSTR LET let_exp IN numexpression cmp_op numexpression
#
# let_exp ::= ID EQUALS numexpression | ID EQUALS numexpression COMMA let_exp
#
# bool ::= TRUE | FALSE
#
# cmp_op ::= LTE | LE | GTE | GE | EQUALS
#   
# Factored out only in grammar write up, because doing this in the real grammar, causes loss of precedence rules
# !binary_math_op! ::= PLUS | MINUS | TIMES | DIVIDE | POWER
# 
#####################################################################
from __future__ import print_function
from parser import Parser
import os
from interface import Interface
from interface import KodiakError
import sys




class KodiakParser(Parser):
    '''Parser for Kodiak Language

       Implements the parser for the Kodiak language and connects directly to Interface.
    '''
    _interface = None

    def __init__(self, **kw):
        '''
           Args:
                interface = instance of interface object, with setting set, if nothing passed default instance created.
                quit_function = a function defining what to do when quit command issued.
                help_function =  a function defining what to do when help command issued.
                read_file_function = a function defining what to do when a file is read.
        '''
        # Merges reserved keywords from base Parser and Kodiak_Parser
        self._reserved = self._reserved + self._reserved_
        for r in self._reserved:
            self.reserved_map[r.lower()] = r
        # Merges tokens from base Parser and Kodiak_Parser    
        self.tokens = self.tokens + self.tokens_
        #Calling superclass constructor
        super(KodiakParser, self).__init__(**kw)

        self._interface = kw.get('interface', Interface())

 ########################################  Lexer definitions ###################################################

    # Reserved keywords
    _reserved_ = ('VAR', 'PARAM', 'IN', 'OBJFN', 'PAVE', 'SET', 'PRECISION', 'RESOLUTION', 'GRANULARITY', 
    'APPROX', 'SAFE', 'INPUT', 'EVAL', 'MODE', 'MINMAX', 'MIN', 'MAX', 'PAVING', 'FULL', 'FIRST', 'STD','BIFURCATION',
    'DFEQ', 'RESET', 'BP', 'EQUILIBRIUM', 'SIN', 'COS', 'TAN', 'ASIN', 'ACOS', 'ATAN','ABS', 'SQRT', 'LN', 'EXP', 'SQ', 
    'DEC', 'RAT', 'CONST', 'VARSELECT', 'DEPTH', 'DEBUG', 'LET', 'CNSTR', 'DEFINE', 'OUTPUT', 'NAME', 'TRUE', 'FALSE', 'FORALL', 'AND', 'OR',
    )

    # Tokens for the _lexer
    tokens_ = _reserved_ + (
    'COMMENT', 'NEWLINE', 'ENDSTATEMENT',
    )

    # Ignored characters
    t_ignore = " \t"

    # Processing newlines to give better info on syntax errors
    def t_newline(self, t):
        r'\n+'
        t.lexer.lineno += t.value.count("\n")
        pass


    t_ENDSTATEMENT = r';'

    reserved_map = { }


    def t_ID(self, t):
        r'[a-zA-Z_][a-zA-Z0-9_]*'
        t.type = self.reserved_map.get(t.value,"ID")
        return t

    def t_comment(self,t):
        r'\#.*'
        pass

    def t_error(self,t):
        print("Illegal character '%s'" % t.value[0])
        t.lexer.skip(1)


 ############################         Parser definitions         #################################################
   
    # precedences and associativity of tokens in increasing order
    # tokens in same level have equal precedence
    precedence = (
         ('nonassoc','LT','GT','GTE','LTE','EQUALS'),
         ('left', 'OR'),
         ('left', 'AND'),
         ('left','PLUS','MINUS'),
         ('left','TIMES','DIVIDE'),
         ('left', 'POWER'),
         ('right','UMINUS'),
         )

    def p_start_expression(self,t):
        '''start_expression : expression'''
        t[0] = t[1]



    def p_expression_comp(self, t):
        '''expression : expression ENDSTATEMENT expression'''
        t[0] = ('COMPOUND', t[1] , t[3])

            
    def p_expression_endstatement(self, t):
        '''expression : expression ENDSTATEMENT'''
        t[0] = t[1]
      
    def p_number_dec(self, t):
        '''number : DEC LPAREN number COMMA number RPAREN'''
        t[0] = (t[1], t[3], t[5])
        
    def p_number_rat(self, t):
        '''number : RAT LPAREN number COMMA NUMBER RPAREN'''
        t[0] = (t[1], t[3], t[5])                    

    def p_number_approx(self, t):
        '''number : APPROX LPAREN number RPAREN'''
        if(self._interface.is_safe_input() is False):
            t[0] = (t[1], t[3])
        else:
            raise KodiakError("Approximate inputs not allowed when safe input is true, input ignored")


    def p_expression_var(self, t):
        'expression : VAR ID IN interval'
        t[0] = ('VAR', t[2], t[4])
        self._interface.var_exp(t[2], t[4])
        
    def p_expression_param(self, t):
        'expression : PARAM ID IN interval'
        t[0] = ('PARAM', t[2], t[4])
        self._interface.var_exp(t[2], t[4], False)
         
        
    def p_expression_const(self, t):
        'expression : CONST ID EQUALS number'
        t[0] = ("CONST", t[2], t[4])
        self._interface.const_exp(t[2],t[4])

    def p_expression_objfn(self, t):
        '''expression : OBJFN numexpression'''
        t[0] = ("OBJFN", t[2])
        self._interface.objfn_exp(t[2])
        
    def p_expression_dfeq(self, t):
        '''expression : DFEQ numexpression'''
        t[0] = ("DFEQ", t[2])
        self._interface.dfeq_exp(t[2])

    def p_expression_constraint(self, t):
        '''expression : CNSTR numexpression cmp_op numexpression'''
        if t[4] != 0:
            t[0] = (t[3], ("-", t[2], t[4]))      
        else:
            t[0] = (t[3], t[2])
        self._interface.constraint_exp(t[0])
        
    def p_expression_set_prec(self, t):
        '''expression : SET PRECISION EQUALS number'''
        t[0] = ("SET", "PRECISION", t[4])
        self._interface.set_precision(t[4])
        
    def p_expression_set_res(self, t):
        '''expression : SET RESOLUTION EQUALS NUMBER'''
        t[0] = ("SET", "RESOLUTION", t[4])
        self._interface.set_resolution(t[4])

    def p_expression_set_granularity(self, t):
        '''expression : SET GRANULARITY EQUALS NUMBER'''
        t[0] = ("SET", "GRANULARITY", t[4])
        self._interface.set_granularity(t[4])

    def p_expression_set_var_res(self, t):
        '''expression : SET RESOLUTION ID EQUALS NUMBER'''
        t[0] = ("SET", "RESOLUTION", t[3], t[5])
        self._interface.set_resolution_var(t[3],t[5])
        
    def p_expression_set_bp(self, t):
        '''expression : SET BP EQUALS bool'''
        t[0] = ("SET", "BP", t[4])
        self._interface.set_bp(t[4])
          
    def p_expression_set_safe(self, t):
        '''expression : SET SAFE INPUT EQUALS bool'''
        t[0] = ("SET", "SAFE", t[5])
        self._interface.set_safe_input(t[5])

        
    def p_expression_set_name(self, t):
        '''expression : SET NAME EQUALS ID'''
        t[0] = ("SET", "ID", t[4])
        self._interface.set_name(t[4])

    def p_expression_set_depth(self, t):
        '''expression : SET DEPTH  EQUALS NUMBER'''
        t[0] = ('SET', 'DEPTH', t[4])
        self._interface.set_depth(t[4])

    def p_expression_set_select(self, t):
        '''expression : SET VARSELECT EQUALS NUMBER'''
        t[0] = ("VARSELECT", t[4])
        self._interface.set_var_select(t[4])
        
    def p_mode_paving_exp(self, t):
        '''expression : SET PAVING MODE EQUALS STD
                      | SET PAVING MODE EQUALS FIRST
                      | SET PAVING MODE EQUALS FULL'''
        t[0] = ("PAVING", "MODE", t[5])
        self._interface.set_paving_mode(t[5])

    def p_expression_set_debug(self, t):
        '''expression : SET DEBUG EQUALS bool'''
        t[0] = ("DEBUG", t[4])
        self._interface.set_debug(t[4])  

    def p_expression_set_output(self, t):
        '''expression : SET OUTPUT EQUALS OUTPUT_FILE_NAME'''
        t[0] = ("OUTPUT", t[4])
        self._interface.set_out_file(t[4])

    def p_expression_reset_output(self, t):
        '''expression : RESET OUTPUT'''
        t[0] = ("OUTPUT", "STDOUT")
        self._interface.set_out_file("")


    def p_numexpression(self, t):
        '''numexpression : ID
                         | number'''
        t[0] = t[1]

    def p_expression_let(self, t):
        '''expression : OBJFN LET let_exp IN  numexpression
                      | DFEQ LET let_exp IN  numexpression
                      | CNSTR LET let_exp IN  numexpression cmp_op numexpression'''
        if(t[1].lower() == "cnstr"):
            t[0] = ("LET", t[1], t[3], (t[6], ("-", t[5], t[7])))
            self._interface.let_exp(t[1], t[3], (t[6], ("-", t[5], t[7])))
        else:
            t[0] = ("LET", t[1], t[3], t[5])
            self._interface.let_exp(t[1], t[3], t[5])

    def p_let_exp(self, t):
        '''let_exp : ID EQUALS numexpression
                   | ID EQUALS numexpression COMMA let_exp'''
        try:
            t[0] = (t[1],t[3],t[5])
        except IndexError:
            t[0] = (t[1], t[3])

    def p_bool(self, t):
        '''bool : TRUE
                | FALSE'''
        t[0] = t[1]


    # operator cannot be extracted into separate rule without loosing precedence!!!
    def p_numexpression_binop(self, t):
        '''numexpression : numexpression PLUS numexpression
                         | numexpression MINUS numexpression
                         | numexpression TIMES numexpression
                         | numexpression DIVIDE numexpression
                         | numexpression POWER numexpression'''
        if t[2] == '+'  : t[0] = ("+", t[1] ,t[3])
        elif t[2] == '-': t[0] = ("-", t[1] ,t[3])
        elif t[2] == '*': t[0] = ("*", t[1] ,t[3])
        elif t[2] == '/': t[0] = ("/", t[1] ,t[3])
        elif t[2] == '^': t[0] = ("^", t[1] ,t[3])

    def p_cmp_op(self, t):
        '''cmp_op : LT
                   | GT
                   | LTE
                   | GTE
                   | EQUALS'''
        t[0] = t[1]

        
    def p_numerical_expression_func(self, t):
        '''numexpression : SIN LPAREN numexpression RPAREN
                         | COS LPAREN numexpression RPAREN
                         | TAN LPAREN numexpression RPAREN
                         | ASIN LPAREN numexpression RPAREN
                         | ACOS LPAREN numexpression RPAREN
                         | ATAN LPAREN numexpression RPAREN
                         | ABS LPAREN numexpression RPAREN
                         | SQ LPAREN numexpression RPAREN
                         | SQRT LPAREN numexpression RPAREN
                         | LN LPAREN numexpression RPAREN
                         | EXP LPAREN numexpression RPAREN''' 
        t[0] = (t[1], t[3])     

    def p_numexpression_uminus(self, t):
        'numexpression : MINUS numexpression %prec UMINUS'
        t[0] = ('u-',t[2])
        

    def p_numexpression_group(self, t):
        'numexpression : LPAREN numexpression RPAREN'
        t[0] = t[2]

        
    def p_expression_pave(self, t):
        'expression : PAVE'
        t[0] = ("PAVE")
        self._interface.pave()
        
    def p_expression_bifurcation(self, t):
        'expression : BIFURCATION'
        t[0] = ("BIFURCATION")
        self._interface.bifurcation()

    def p_expression_equilibrium(self, t):
        'expression : EQUILIBRIUM'
        t[0] = ("EQUILIBRIUM")
        self._interface.bifurcation(True)
        
    def p_expression_min(self, t):
        'expression : MIN'
        t[0] = ("MIN")
        self._interface.minmax("min")
        
    def p_expression_max(self, t):
        'expression : MAX'
        t[0] = ("MAX")
        self._interface.minmax("max")    
        
    def p_expression_minmax(self, t):
        'expression : MINMAX'
        t[0] = ("MINMAX")
        self._interface.minmax()

    def p_expression_reset(self, t):
        'expression : RESET'
        t[0] = ("RESET")
        self._interface.reset_env()

    def p_expression_def(self, t):
        'expression : DEFINE ID EQUALS numexpression'
        t[0] = ("DEFINE", t[2], t[4])
        self._interface.def_exp(t[2], t[4])

    def p_bool_op(self,t):
        '''bool_op : AND
                      | OR'''
        t[0] = t[1]

    def p_group_bool(self,t):
        '''b_exp : LPAREN b_exp RPAREN'''
        t[0] = t[2]

    def p_b_exp(self,t):
        '''b_exp : numexpression cmp_op numexpression
                 | b_exp AND b_exp
                 | b_exp OR b_exp'''
        try:
            t[0] = (t[4], (t[2],t[1],t[3]), (t[5], t[4], t[6]))
        except IndexError:
            t[0] = (t[2], t[1], t[3])

    def p_save_paving(self,t):
        '''expression : SAVE PAVING ID'''
        t[0] = (t[1], t[2], t[3])
        self._interface.save_paving(t[3])

    def p_load_paving(self,t):
        '''expression : LOAD PAVING ID'''
        t[0] = (t[1], t[2], t[3])
        self._interface.load_paving(t[3])

    def p_plot(self,t):
        '''expression : PLOT ID ID ID
                      | PLOT ID ID ID ID'''
        try:
            self._interface.plot(t[2],(t[3], t[4], t[5]))
        except IndexError:
            self._interface.plot(t[2],(t[3],t[4]))



    def p_expression_forall(self, t):
        '''expression : FORALL b_exp'''
        t[0] = (t[1], t[2])
        print (t[0])

        
    def p_error(self, t):
        if t == None:
            print ("Syntax ERROR: invalid syntax", file=sys.stderr)
        else:
            print ("Syntax ERROR: on input {0} at line {1} column {2}".format(str(t.value), str(t.lineno), str(t.lexpos)), file=sys.stderr)
            # raise SyntaxError("Syntax error on input %s at line %s column %s" % (str(t.value), str(t.lineno), str(t.lexpos)))


