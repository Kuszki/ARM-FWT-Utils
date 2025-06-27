/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  ARM-FWT-Test -- FWT implementation for ARM using CMSIS-DSP library     *
 *  Copyright (C) 2025  Łukasz "Kuszki" Dróżdż  lukasz.kuszki@gmail.com    *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the  Free Software Foundation, either  version 3 of the  License, or   *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This  program  is  distributed  in the hope  that it will be useful,   *
 *  but WITHOUT ANY  WARRANTY;  without  even  the  implied  warranty of   *
 *  MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the   *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have  received a copy  of the  GNU General Public License   *
 *  along with this program. If not, see http://www.gnu.org/licenses/.     *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "hdf_helper.h"

#include <hdf5.h>

int hdf_load_matrix(const char* filename, const char* varname, hdf_matrix* out)
{
	if (!filename || !varname || !out) return 1;

	hid_t file_id = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);
	if (file_id < 0) return 2;

	hid_t dataset_id = H5Dopen(file_id, varname, H5P_DEFAULT);
	if (dataset_id < 0)
	{
		H5Fclose(file_id);
		return 3;
	}

	hid_t space_id = H5Dget_space(dataset_id);
	if (space_id < 0)
	{
		H5Dclose(dataset_id);
		H5Fclose(file_id);
		return 4;
	}

	int ndims = H5Sget_simple_extent_ndims(space_id);
	if (ndims != 2)
	{
		H5Sclose(space_id);
		H5Dclose(dataset_id);
		H5Fclose(file_id);
		return 5;
	}

	hsize_t dims[2];
	H5Sget_simple_extent_dims(space_id, dims, nullptr);

	size_t rows = (size_t) dims[0];
	size_t cols = (size_t) dims[1];
	size_t total = rows * cols;

	float32_t* data = malloc(sizeof(float32_t) * total);
	if (!data)
	{
		H5Sclose(space_id);
		H5Dclose(dataset_id);
		H5Fclose(file_id);
		return 6;
	}

	herr_t status = H5Dread(dataset_id, H5T_NATIVE_FLOAT,
					    H5S_ALL, H5S_ALL, H5P_DEFAULT,
					    data);
	if (status < 0)
	{
		free(data);
		H5Sclose(space_id);
		H5Dclose(dataset_id);
		H5Fclose(file_id);
		return 7;
	}

	out->data = data;
	out->rows = rows;
	out->cols = cols;

	H5Sclose(space_id);
	H5Dclose(dataset_id);
	H5Fclose(file_id);

	return 0;
}

void hdf_free_matrix(hdf_matrix* matrix)
{
	if (!matrix->data) return;
	else free(matrix->data);

	matrix->data = nullptr;
	matrix->cols = 0;
	matrix->rows = 0;
}
