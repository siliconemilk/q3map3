
/* ON FUNCTION USAGE - Do not use these OpenCL functions on data in HOST code unless
 * you are copying large amounts of data in one fell swoop. 
 * 
 * These functions are meant for use with data already existing in OpenCL device
 * memory. 
 * Limit read/write operations across the bus whenever possible or you could find
 * yourself looking at a DECREASE in performance! 
 *
 */ 

//-------------------------------------
// Vector Multiply-Add
//  - Adds product of S and B with A and stores results in output
//-------------------------------------
void VectorMA(read_only float4 *a, read_only float s, 
			  read_only float4 *b, write_only float4 *output){
	*output = (*a + (s * (*b))); 
} 
void VectorSubtract(read_only float4 *a, read_only float4 *b, write_only float4 *output){
	*output = (*a) - (*b); 
} 
float4 VectorAdd(read_only float4 a, read_only float4 b){
	/*output[0] = a[0] + b[0]; 
	output[1] = a[1] + b[1]; 
	output[2] = a[2] + b[2]; 
	output[3] = a[3] + b[3]; */
	//float *v1 = a; 
	//float *v2 = b; 
	return (a) + (b); 
} 
void VectorCopy(read_only float4 *a, write_only float4 *output){
	*output = *a; 
} 
void VectorScale(read_only float4 *a, read_only float4 s, write_only float4 *output){
	*output = (*a) * s; 
} 

//-------------------------------------
// Vector Midpoint
//  - Calculates the mean of A + B and stores the result in output
//-------------------------------------
void VectorMid(read_only float4 *a, read_only float4 *b, write_only float4 *output){
	*output = (((*a) + (*b)) * 0.5f); 
} 
void VectorNegative(read_only float4 *a, write_only float4 *b){
	*b = -(*a); 
}
void VectorClear(write_only float4 *a){
	*a = 0.0f; 
} 

