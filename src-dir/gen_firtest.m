C = [ 1 2 3 4 ];
C = C / sum(C);

B = [ 5 2 1 3 ];
B = B / sum(B);

X = [ 1 8 5 4 6 2 3 0 ]';

sL = sH = zeros(1, length(C) - 1);

[L, sL] = filter(flip(C), [1], X, sL);
[H, sH] = filter(flip(B), [1], X, sH);

[L, sL] = filter(flip(C), [1], X, sL);
[H, sH] = filter(flip(B), [1], X, sH);

save("-hdf5", "../data-dir/filter_out.hdf", "C", "B", "X", "H", "L");
