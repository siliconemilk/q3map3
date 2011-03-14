/* The OpenCL kernels for q3map2's radiosity functions found in the light compile stage */ 
#include "vector_math.cl" 

__kernel void vector_add_gpu(
    __global const float4 *src_a,
    __global const float4 *src_b,
	__global float4 *result,
	const int num)
{
	const int idx = get_global_id(0); 

	/*if(idx < num){
		result[idx] = src_a[idx] * src_b[idx]; 
		printf(result[idx]);
	}*/

	if(idx < num){ 
		result[idx] = VectorAdd(src_a[idx], src_b[idx]);
		//printf(result[idx]);
	} 

}