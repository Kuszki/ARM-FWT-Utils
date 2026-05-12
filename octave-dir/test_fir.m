clear;

addpath("../fwt-utils/");

pkg load ltfat

C = [ 1+sqrt(3) 3+sqrt(3) 3-sqrt(3) 1-sqrt(3) ] / 4 / sqrt(2);
B = fwt_gen_b(C);

X = [ 1 8 5 4 6 2 3 0 ]';

z1 = zeros(length(C)-1, 1);

[Y, z1] = filter(C, [1], X, z1)
[Y, z1] = filter(C, [1], X, z1)
[Y, z1] = filter(C, [1], X, z1)
#[Y, z1] = filter(B, 1, X, [])'
