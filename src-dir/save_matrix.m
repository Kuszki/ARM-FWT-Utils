function A = save_matrix(name, lvl, len)

	A = lin_ident(@(x) fwt(x, name, lvl), len);
	N = sprintf("../data-dir/%s_%d_%d.hdf", name, lvl, len);

	save("-hdf5", N, "A");

end
