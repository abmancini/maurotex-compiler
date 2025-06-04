beginfig(-1);

draw (1cm,0cm)--(1cm,1cm)--(0cm,1cm)--(0cm,0cm)--(1cm,0cm);

a  = 4.0cm;
b  = 3.0cm;
z0 = (0,0);
z1 = -z3 = (a,0);
z2 = -z4 = (0,b);
draw z1..z2..z3..z4..cycle;
draw z1--z0--z2;

endfig;

end
