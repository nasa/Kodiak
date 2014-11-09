from kodiak_lexer import *
from kodiak_parser import *




if __name__ == "__main__":
    import sys
    tests = []
#     tests.append("var x in [5,4]")
#     tests.append("var x in [4,5.0]")
#     tests.append("var x in [4.2,-0.1]")
#     tests.append("var x in [-4,-1];")
#     tests.append('''var x in [4,-5]; 
#     var y in [3,5]''')
#     tests.append('''var x in [4,5000.123];; 
#     var y in [3,5]''')
#      
#     tests.append('''var x in [4,5] ;
#     var y in [3,5];
#     var z in [5,4]''')
#     tests.append('''poly x = y+5+3+2;''')
#     tests.append('''
#       
#     ''')
#     tests.append('''
#      
#      
#      
#     poly x = 4;
#      
#      
#      
#     var x in [4,5];
#      
#      
#     ''')
#      
#     tests.append("var x in [(4+2)*4, 2+4*2]")
#     tests.append('''var x in 
#     [(4+2)*4,
#      2+4*2];
#      var y in [5,4]''')
#     
#     tests.append("set precision = 5")
#     tests.append("set precision = 0.1")
#     tests.append("set resolution = 5")
#     tests.append("set resolution = 0.1")
#     tests.append("set resolution")#SHOULD BE ERROR
#     tests.append("poly x = approx(5)")
#     tests.append("poly x = approx(0.1)")
#     tests.append("set safe input = true")
#     tests.append("set safe input = false")
#     tests.append("poly x = -x")
#     tests.append("poly x = -(x + 4) * 5 * x ^ 3 - 2 * (5+y)")
#    tests.append("poly x = approx(5.2)")
#    tests.append("x^2 + -5*x + 6 == 0")
#    tests.append("PAVE;")
    # tests.append("set temp = 2")
    tests.append("set precision = -2")   
    tests.append("objfn = dec(2,2)")
    tests.append("objfn = dec(2,-2)")
    tests.append("objfn = rat(2,2)")
    # tests.append("objfn = rat(2,-2)")
    tests.append("objfn = approx(-2.2)")
    tests.append("var x in [approx(-2.2),10]")
    tests.append("set resolution = 0.02")
    tests.append("cnstr = x+2 = 0")
    tests.append("cnstr = x+rat(1,2) = 0")
    tests.append("cnstr = let x=5+y in x+y = 0")
    tests.append("objfn = let x=5+y in x+y")
    tests.append("dfeq = let x=5+y in x+y")

#     tests.append('''
#     ;''')
    
    for test in tests:
        t = yacc.parse(test)
        print t
        
else:
    while 1:
        try:
            s = raw_input('Kodiak > ')
        except EOFError:
            break
        yacc.parse(s)


