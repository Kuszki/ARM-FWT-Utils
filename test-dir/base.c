#include "arm_math.h"

const size_t N = 1024;

int main(int argc, char* argv[])
{
	float32_t A[N][N];
	float32_t X[N];
	float32_t Y[N];
	float32_t M[N / 2];

	arm_matrix_instance_f32 mat_A;
	arm_mat_init_f32(&mat_A, N, N, (float32_t*) A);

	arm_matrix_instance_f32 mat_X;
	arm_mat_init_f32(&mat_X, N, 1, X);

	arm_matrix_instance_f32 mat_Y;
	arm_mat_init_f32(&mat_Y, N, 1, Y);

	arm_rfft_fast_instance_f32 S;
	arm_rfft_fast_init_f32(&S, N);
	
	arm_mat_mult_f32(&mat_A, &mat_X, &mat_Y);
	arm_rfft_fast_f32(&S, X, Y, 0);
	arm_cmplx_mag_f32(Y, M, N / 2);
	
	return 0;
}
