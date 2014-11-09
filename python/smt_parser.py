import ply.lex as lex
import ply.yacc as yacc
from parser import Parser
from interface import Interface
class SMTParser(Parser):
    '''SMTParser class'''


    def __init__(self, **kw):
        for r in self._reserved:
            self.reserved_map[r.lower()] = r
        #Calling superclass constructor
        self.tokens = self.tokens + self.tokens_
        super(SMTParser, self).__init__(**kw)
        self._interface = kw.get('interface', Interface())
 ########################################  Lexer definitions ###################################################

    # Reserved keywords
    _reserved = ('VAR', 
    )

    # Tokens for the _lexer
    tokens_ = _reserved + ('CHECKSAT',
    )
	# tokens_ = _reserved + ()		

    # Ignored characters
    t_ignore = " \t"

    # Processing newlines to give better info on syntax errors
    def t_newline(self,t):
        r'\n+'
        t._lexer.lineno += t.value.count("\n")
        pass

    reserved_map = { }


    def t_CHECKSAT(self,t):
    	r'check-sat'
    	return t

    def t_FILE_NAME(self,t):
        r'[a-zA-Z0-9_][a-zA-Z0-9_\-]*\.[a-zA-Z0-9_]*'
        return t

    def t_ID(self,t):
        r'[a-zA-Z_][a-zA-Z0-9_]*'
        t.type = self._reserved_map.get(t.value,"ID")
        return t

    def t_HEX (self,t):
        r'0x[0-9A-Fa-f]*(\.[0-9A-Fa-f]+)?(p-?\d+)?'
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
         ('nonassoc','LT','GT','GTE','LTE'),
         ('left','PLUS','MINUS'),
         ('left','TIMES','DIVIDE'),
         ('left', 'POWER'),
         # ('right','UMINUS'),
         )



    def p_start_expression(self, t):
        '''start_expression : LPAREN expression RPAREN'''
        t[0] = (t[2])
        print(t[0])

    def p_expression_id(self, t):
    	'''expression : VAR ID interval'''
    	t[0] = (t[1], t[2])

    def p_expression_math(self,t):
    	'''expression : PLUS number number
    				  | MINUS number number
    				  | TIMES number number
    				  | DIVIDE number number
    				  | POWER number number'''
    	t[0] = (t[1], t[2], t[3])

    def p_interval(self, t):
        'interval : LBRACKET number COMMA number RBRACKET'
        t[0] = ("INTERVAL", t[2], t[4])    

    def p_expression_check(self,t):
    	'''expression : CHECKSAT'''
    	t[0] = t[1]
        
    def p_error(self, t):
        if t == None:
            print ("Input is empty")
        else:
            print ("Syntax error on input %s at line %s column %s" % (str(t.value), str(t.lineno), str(t.lexpos)))
            # raise SyntaxError("Syntax error on input %s at line %s column %s" % (str(t.value), str(t.lineno), str(t.lexpos)))
