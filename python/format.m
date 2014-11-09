%example of passing a bifurcation problem to Kodiak
x.res = 0.001;
x.prec = 0;
x.vars = [{'x', 0,8}; {'y', 0, 8}; {'z', 0, 29};];
x.params = [{'sigma', 3,3};{'rho', 1, 30};{'beta', 1, 2}];
x.dfeqs = {'sigma*(y-x)', 'x*(rho-z)-y', 'x*y - beta*z'};
x.cnstrs = {'x > 0'};
y = bifurcation(x)


