N = 128;

X_max =  1e5;
X_min = -2e4;

A_max =  1e3;
A_min = -2e2;

X_ran = X_max - X_min;
A_ran = A_max - A_min;

X = X_ran * rand(1, N) + X_min;
A = A_ran * rand(N, N) + A_min;

Y = X*A;

save("-hdf5", "../data-dir/mul_out.hdf", "A", "X", "Y");
