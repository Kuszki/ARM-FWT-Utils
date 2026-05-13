clear;

addpath("../fwt-utils/");

pkg load ltfat

C = [ 1+sqrt(3) 3+sqrt(3) 3-sqrt(3) 1-sqrt(3) ] / 4 / sqrt(2);
B = fwt_gen_b(C);

#C= flip(C)
#B= flip(B)


X = [ 0 1 2 3 0 1 2 3 1 2 3 4 5 7 8 9 ] #[ 1 8 5 4 6 2 3 0 ]';
Y = ufwt(X, "db2", 1)

zL1 = zH1 = zL2 = zH2 = zeros(length(C) - 1, 1);

for i = 1:5

[L1, zL1] = filter(C, 1, X, zL1);
[H1, zH1] = filter(B, 1, X, zH1);

#[L2, zL2] = filter(C, 1, L1, zL2);
#[H2, zH2] = filter(B, 1, L1, zH2);

#disp([H1', L1'])

end

clf;
hold on;
plot(L1)
plot(Y(:,1))
