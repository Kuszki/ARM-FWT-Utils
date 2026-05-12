function [b] = fwt_gen_b(coefs)

  N = length(coefs);

  for i = 0 : N - 1

    n = mod(N - i - 1, N);
    b(i+1) = (-1)^i * coefs(n+1);

  end

end

