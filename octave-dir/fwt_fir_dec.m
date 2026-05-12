function [y] = fwt_fir_dec(x, coefs)

  N = length(x);
  y = zeros(1, N / 2);

  for i = 0 : N / 2 - 1

#    printf("y_%d = ", i)

    for j = 0 : length(coefs) - 1

      n = mod(2*i + j, N);
#      printf("c_%d * x_%d + ", j, n)
      y(i+1) = y(i+1) + coefs(j+1) * x(n+1);

    end

#    printf("\n");

  end

  y = y / sqrt(2);

end
