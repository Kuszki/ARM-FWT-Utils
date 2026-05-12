function [y] = fwt_mallat(x, n, c, b)

  S = x;
  y = [];

  for i = 1:n

    T = fwt_fir_dec(S, b);
    S = fwt_fir_dec(S, c);

    y = [ T y ];

  end

  y = [ S y ];

end
