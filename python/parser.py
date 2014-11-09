from __future__ import print_function
from interface import KodiakError
import ply.lex as lex
import ply.yacc as yacc
import sys

class Parser(object):
    """Base class for a lexer/parser that has the rules defined as methods

        An implementation of parser and lexer from PLY(Python Lex Yacc)
        Methods and fields starting with prefix t_ define tokens
        Methods starting with prefix p_ define rules

        Rules defined as field have precedence according to their length.
        Rules defined as methods have precedence according to the order in which they are defined.

        Attributes:
                precedence: used by PLY to specify precedence for _parser rules.
                tokens: used by ply to hold tokens. 

    """
    precedence = ()
    tokens = ()

    _reserved = ()
    _lexer = None
    _parser = None 
    _quit_function = None
    _read_file_function = None
    _help_function = None


    def __init__(self, **kw):
        '''
           Args:
                interface = instance of interface object, with setting set, if nothing passed default instance created.
                quit_function = a function defining what to do when quit command issued
                help_function =  a function defining what to do when help command issued
                read_file_function = a function defining what to do when a file is read. Not used currently.
        '''
        self.debug = kw.get('debug', 0)
        self.names = { }
        modname = "kodiak_parser"
        self.debugfile = modname + ".dbg"
        self.tabmodule = modname + "_" + "parsetab"

        self._quit_function = kw.get('quit_function', None)
        self._help_function = kw.get('help_function', None)
        self._read_file_function = kw.get('read_file_function', None)

        # Create the _lexer and _parser
        self._lexer = lex.lex(module=self, debug=self.debug)
        self._parser = yacc.yacc(module=self,
                  debug=self.debug,
                  debugfile=self.debugfile,
                  tabmodule=self.tabmodule, errorlog=yacc.NullLogger())

    def parse(self, input_string):
        '''Parses the input.

           Args:
                input_string: any length string containing input to interface.

           Retuns:
                On succesful parse returns True.
        '''
        return self._parser.parse(input_string) is not None

    def lex(self, input_string):
        '''Used for testing the lexer

            Lexes the input_string and prints the produced tokens.

            Args:
                input_string: a string for lexer.
        '''
        self._lexer.input(input_string)
        for tok in self._lexer:
            print (tok)

    _reserved = ('QUIT', 'EXIT', 'FILE', 'HELP', 'SAVE', 'LOAD', 'PLOT',
    )

    # Tokens general to all _parsers used in kodiak
    tokens = _reserved + ('INPUT_FILE_NAME', 'OUTPUT_FILE_NAME', 'NUMBER', 'PLUS', 'MINUS',
    'TIMES', 'DIVIDE', 'POWER', 'EQUALS',
    'LPAREN','RPAREN', 'LBRACKET', 'RBRACKET',
    'LT', 'GT', 'LTE', 'GTE', 'COMMA', 'ID', 'HEX',
    )

    t_PLUS     = r'\+'
    t_MINUS    = r'\-'
    t_TIMES    = r'\*'
    t_DIVIDE   = r'/'
    t_POWER    = r'\^'
    t_EQUALS   = r'='
    t_LPAREN   = r'\('
    t_RPAREN   = r'\)'
    t_LBRACKET = r'\['
    t_RBRACKET = r'\]'
    t_LT       = r'<'
    t_GT       = r'>'
    t_LTE      = r'<='
    t_GTE      = r'>='
    t_COMMA    = r','

    # Matches any filenames with number, /, \, -, _, (, ) and letter ending with .kdk'''
    def t_INPUT_FILE_NAME(self,t):
        r'[a-zA-Z0-9_\-/\\\(\)]*\.kdk$'
        return t

    def t_OUTPUT_FILE_NAME(self,t):
        r'[a-zA-Z0-9_\-/\\\(\)]*\.[a-zA-Z]+'
        return t

    def t_NUMBER(self,t):
        r'\d+\.?\d*'
        try:
            t.value = int(t.value)
            if(t.value > (sys.maxsize / 2) ** 0.5 - 1):
                raise KodiakError("Number too large, overflow occured.")
        except ValueError:
            t.value = float(t.value)
        return t

    def t_HEX (self,t):
        r'0x[0-9A-Fa-f]*(\.[0-9A-Fa-f]+)?(p-?\d+)?'
        return t
    
    #Grammar rules general to all parsers in Kodiak    
    def p_statement_exp(self,t):
        '''statement : start_expression'''
        t[0] = t[1]

    def p_interval(self, t):
        '''interval : LBRACKET number COMMA number RBRACKET'''
        t[0] = ("INTERVAL", t[2], t[4])   

    def p_number(self,t):
        '''number : NUMBER
                  | MINUS NUMBER
                  | HEX'''
        try:
            t[0] = (-t[2])
        except IndexError:
            t[0] = t[1]

    def p_read_file(self,t):
        '''expression : FILE INPUT_FILE_NAME'''
        t[0] = t[2]
        self._read_file_function(t[2], False)

    def p_quit(self,t):
        '''expression : QUIT
                      | EXIT'''
        t[0] = t[1]
        self._quit_function()

    def p_help(self,t):
        '''expression : HELP'''
        t[0] = t[1]
        self._help_function()
