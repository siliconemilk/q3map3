
#ifndef Q3MAP3_CL_H
#define Q3MAP3_CL_H

#include "CL/cl.h"

#define PLATFORM_IDS_SIZE 1 
#define DEVICE_TYPE CL_DEVICE_TYPE_GPU /*default to gpu device. ocl can also use CPU as a device type*/ 

/*kernels are compiled at run-time from strings. Each index will contain a series of kernels to be compiled. 
  The following is a "dynamic array" so we don't have to load every single file in one go.*/ 
typedef struct{ 
	int num; /*number of files loaded in to the source string*/ 
	int size; 
	char *source; /* Each file as string stored here */  
} t_source; 

/* The "array" we will work with for kernel shenanigans */
t_source *kernel_source;

cl_int           clerror; /*opencl error handling*/ 
cl_platform_id   clplatform; 
cl_int           num_clplatforms; /*number of ocl platforms detected*/ 
cl_context       clcontext; 
cl_command_queue clqueue; 
cl_device_id     cldevice; 
cl_event         clevent; 

cl_program       clprogram; /*ocl program which consists of all loaded kernels*/

/***********************************
* HARD-CODED MEMORY BUFFERS AND KERNEL OBJECTS
* Replace with flexible solution after testing
************************************/
static const int MAGIC_SIZE = 16384; 
size_t local_ws;
size_t global_ws;
size_t awesome; 
#define MEM_SIZE (sizeof(cl_float4)*MAGIC_SIZE) /*4 * 4096 bytes of memory per argument... max size of 4096 for float arrays*/
/*host memory*/
cl_float4 src_a_h[16384]; 
cl_float4 src_b_h[16384]; 
cl_float4 res_h[16384]; 
/* device memory*/
cl_mem src_a; 
cl_mem src_b; 
cl_mem res; 
cl_kernel magic; 
int stupid; 
cl_ulong start, end; 
float total; 


typedef struct{
	cl_mem mem[10]; /*memory buffer array for kernel object*/

	size_t local_ws; /*local work size*/
	size_t global_ws; /*global work size*/

	cl_kernel kernel; /*the function to be executed*/
}t_kernel; 

t_kernel kernels[256]; /*An array of *COMPILED* and ready-to-be-executed OpenCL functions.*/
/* ^ Allows for up to 256 functions. Increase as needed.*/



/*OpenCL Helper Functions to keep the clutter out of q3map2's source*/ 
qboolean InitOpenCL( void ); 
qboolean GetPlatform(cl_platform_id *platform); /*We don't even need to bother passing cl_platform_id as an argument*/
qboolean GetDevice( void ); 
qboolean CreateContext( void ); 
qboolean CreateCommandQueue( void ); 

void CleanOpenCL( void ); /*clean up memory and release ocl objects*/ 

qboolean LoadProgramSource(char *filename); /*Load a .cl file to a single string. Return value stored in kernels[]*/ 
void BuildAllProgramSource( void ); /*builds all loaded .cl files in to executable code*/

void PushKernel(char *kernel, unsigned int *length); /*push a kernel on to the array*/ 
void do_stuff(); 

#endif