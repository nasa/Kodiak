function b = bifurcation(i)
%BIFURCATION  find a bifurcation of the specified problem by using Kodiak library.
%
%   B = BIFURCATION(I)
%
%   I is a structure with all the values which are going to be passed to Kodiak non-linear solver.
%
%   Fields can be specified in any order and unnecessary field can be omitted.
%
%   It can contain the following fields:
%   I.prec - to set the precision in Kodiak. Ex. -2;
%   I.res - to set resolution in Kodiak. Ex. 0.001
%   I.vars - for variables. Ex. [{'x', 0,8}; {'y', 0, 8}; {'z', 0, 29};];
%   I.params - for parameters. Ex. [{'sigma', 3,3};{'rho', 1, 30};{'beta', 1, 2}];
%   I.dfeqs - for differential equations. Ex. {'sigma*(y-x)', 'x*(rho-z)-y', 'x*y - beta*z'};
%   I.cnstrs - for constraints. Ex. {'x^2 - 5 > 0', 'sigma/z > x'};
%
%   Support for differential equations in ODE format will be added in the future.
%
%   Output B is a a matrix of structures of format:
%   B(N).type - a descriptions of the type of boxes
%   B(N).boxes - all the boxes of this type produced by Kodiak
%   Where N is the number of types of boxes produced by Kodiak
%
%   It can contain different number of types depending on the output from Kodiak
%
%

b = kodiak('b', i);
end