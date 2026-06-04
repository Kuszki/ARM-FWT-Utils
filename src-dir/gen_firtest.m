C = [ 1 2 3 4 8 -7 -9 -1 6 7 ];
C = C / sum(C);

B = [ 5 2 1 3 9 8 1 -7 -6 -8 ];
B = B / sum(B);

X = int16(rand(100, 1)*25);

sL = sH = zeros(1, length(C) - 1);

[L, sL] = filter(flip(C), [1], X, sL);
[H, sH] = filter(flip(B), [1], X, sH);

[L, sL] = filter(flip(C), [1], X, sL);
[H, sH] = filter(flip(B), [1], X, sH);

save("-hdf5", "../data-dir/filter_out.hdf", "C", "B", "X", "H", "L");
