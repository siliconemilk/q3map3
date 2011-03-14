#include "q3map2.h" 
#include "q3map3_cl.h" 

/*************************************
* Kernel Management Functions
*************************************/
void PrintKernels(){
	Sys_Printf("%s\n", kernel_source->source); 
	Sys_Printf("Kernel Heap Size: %d\n\n", kernel_source->size * sizeof(char));
}

void PushKernel(char *kernel, unsigned int *length){
    unsigned int len = *length;
	unsigned int size = kernel_source->size; 

/*check to see if our destination is large enough to hold current data + data we want to add. 
  If not, realloc double the amount of current memory until there is room*/
	while(kernel_source->size < size + len){
		kernel_source->source = (char*)realloc(kernel_source->source, (kernel_source->size *= 2) * sizeof(char));
		Sys_FPrintf(SYS_VRB, "[Reallocating kernel_source->source]\n    -<size = %d>-\n", kernel_source->size); 
	}

	/*if we have no string data loaded, copy the file to the string. 
	  Otherwise, append the file to the string*/
	if(kernel_source->num == 0){
	    strcpy(kernel_source->source, kernel);
	}else{
		strcat(kernel_source->source, kernel); 
	}

	kernel_source->num++; 

}


/*Main OCL Helper Function - Call this on initialization to get started*/ 
qboolean InitOpenCL(){
	int i, j; 

	if(GetPlatform(&clplatform) != qtrue){
		return qfalse; 
	}
	if(GetDevice() != qtrue){
		return qfalse; 
	}
	if(CreateContext() != qtrue){
		return qfalse; 
	}
	if(CreateCommandQueue() != qtrue){
		return qfalse; 
	}

	/*DONT FORGET TO FREE ON CLEANUP!*/

	/*note that we can use calloc() because we have no floating point variables to worry about. 
	  Initialize each field manually if unsure if the var type's "zeroed" form is all-bits-zero*/

	kernel_source = (t_source*)calloc(sizeof(t_source), 1); /*zero init our kernel structure*/
	/*Give the char array some room for the source to be loaded in to*/
	kernel_source->source = (char*)calloc((kernel_source->size = 4) * sizeof(char), sizeof(char)); 
	
	/*Zero init all fields of all kernel structures in our kernels array*/ 
	for(i = 0; i < sizeof(kernels) / sizeof(t_kernel); i++){
		for(j = 0; j < sizeof(kernels[i].mem) / sizeof(cl_mem); j++){
			kernels[i].mem[j] = NULL; 
		}
		kernels[i].global_ws = 0; 
		kernels[i].local_ws = 0; 
		kernels[i].kernel = NULL; 
	}

	return qtrue; 
}

//**********************************************
// Miscellaneous OCL Helper Functions
//**********************************************

qboolean GetPlatform(cl_platform_id *platform){
	clerror = clGetPlatformIDs(PLATFORM_IDS_SIZE, platform, &num_clplatforms); 
	if(clerror != CL_SUCCESS){
		Sys_Printf("Error getting platform id '%d'\n", clerror); 
		return qfalse; 
	}
	
	Sys_FPrintf(SYS_VRB, "-< ocl platform id - OK! >- \n"); 

	return qtrue; 
} 

qboolean GetDevice( void ){ 
	clerror = clGetDeviceIDs(clplatform, DEVICE_TYPE, 1, &cldevice, NULL); 
	if(clerror != CL_SUCCESS){
		Sys_Printf("Error getting device id '%d'\n", clerror); 
		return qfalse; 
	} 

	Sys_FPrintf(SYS_VRB, "-< ocl device id - OK! >- \n"); 

	return qtrue; 
} 

qboolean CreateContext( void ){ 
	clcontext = clCreateContext(0, 1, &cldevice, NULL, NULL, &clerror); 
	if(clerror != CL_SUCCESS){ 
		Sys_Printf("Error creating ocl context '%d' \n", clerror); 
		return qfalse; 
	}

	Sys_FPrintf(SYS_VRB, "-< ocl context created - OK! >- \n"); 

	return qtrue; 
} 

qboolean CreateCommandQueue( void ){ 
	clqueue = clCreateCommandQueue(clcontext, cldevice, CL_QUEUE_PROFILING_ENABLE, &clerror); 
	if(clerror != CL_SUCCESS){
		Sys_Printf("Error creating ocl command queue '%d' \n", clerror); 
		return qfalse; 
	}

	Sys_FPrintf(SYS_VRB, "-< ocl command queue created - OK! >- \n\n"); /*extra newline for formatting reasons*/

	return qtrue; 
} 


qboolean LoadProgramSource(char *filename){
	FILE *fp; 
	int kernel_length; 
	char *result; 

	fp = fopen(filename, "r"); 

	if(!fp){
		Sys_Printf("=== ERROR === \n Unable to load kernel '%s'. Falling back to CPU.\n", filename); 
		gpu = qfalse; 

		CleanOpenCL(); /*release ocl objects that have been initialized so far*/
		return qfalse; 
	}

	/*note success of file reading*/
	Sys_FPrintf(SYS_VRB, "--- Opened %s successfully! ---\n", filename); 

	fseek(fp, 0, SEEK_END); 
    kernel_length = ftell(fp); 
	rewind(fp); 

	result = (char*)calloc(kernel_length, sizeof(char));
	fread(result, 1, kernel_length, fp); 

	/*note success of malloc and read*/ 
	Sys_FPrintf(SYS_VRB, "    [Success On Malloc and Read]\n\n"); 
	PushKernel(result, &kernel_length); 

	fclose(fp); 

	return qtrue; 
}

void BuildAllProgramSource( void ){
	size_t len; 
	FILE *log; 
	char *buffer; 

	clprogram = clCreateProgramWithSource(clcontext, 1, (const char **)&(kernel_source->source), NULL, &clerror); 
	

	if(!clprogram){
		Sys_Printf("=== ERROR ===\n Unable to create OpenCL program object. Falling back to CPU.\n"); 
		
		gpu = qfalse; 
		CleanOpenCL(); /*clean up all the ocl-related memory we've allocated so far*/
		return; /*early out*/
	}

	clerror = clBuildProgram(clprogram, 0, NULL, NULL, NULL, NULL); 
	if(clerror != CL_SUCCESS){
		Sys_Printf("=== ERROR ===\n Unable to build OpenCL program. Falling back to CPU.\n"); 
		Sys_Printf("Saving error log to 'ocl_error_log.log'.\n"); 


		clGetProgramBuildInfo(clprogram, cldevice, CL_PROGRAM_BUILD_LOG, 0, NULL, &len); /*first call for size*/
		buffer = (char*)calloc(len, sizeof(char)); /*zero init buffer*/
		clGetProgramBuildInfo(clprogram, cldevice, CL_PROGRAM_BUILD_LOG, len, buffer, NULL); /*second call for log*/



		/*time to dump the errors to an external log*/
        log = fopen("ocl_error_log.log", "wt"); 
		if(!log){
			Sys_Printf("Can't write to 'ocl_error_log.log'.\n    [You're pretty screwed]\n"); 
		}

		Sys_Printf("%s\n", buffer); 
		fwrite(buffer, sizeof(buffer), sizeof(char), log);  

		fclose(log); 
		
		free(buffer); /*don't forget to clean up our buffer*/

		gpu = qfalse;
		CleanOpenCL(); /*clean up all the ocl-related memory we've allocated so far*/
		return; 
	}

	Sys_Printf("OpenCL Program Creation and Compilation Successfull!\n"); 
}

void CleanOpenCL( void ){
	cl_uint references; /*number of references for error handling*/
	int itr, itr_end; 

	itr_end = sizeof(kernels) / sizeof(t_kernel); 
	//PrintKernels(); 

	/*clean up the memory allocated for our kernel source. 
	  First the string, then the structure*/
	free(kernel_source->source); 
	free(kernel_source);

	/*Just want to make sure these are valid to release. 
	  Note: clRelease____ functions merely decrease the reference count
	  of the object being "released". The objects aren't truely deleted
	  until ALL objects referring to it are freed up as well*/

	/*release all cl_kernel objects*/
	for(itr = 0; itr < itr_end; itr++){
	    clerror = clReleaseKernel(kernels[itr].kernel); 
	    if(clerror != CL_SUCCESS){
			//Sys_Printf("WARNING: Unable to free ocl kernel object for unknown reasons!\nIteration: %d\n", itr); 
		}
	}

	clerror = clReleaseProgram(clprogram); 
	if(clerror != CL_SUCCESS){
		Error("\tInvalid program object.\n\tHave all the kernels been destroyed already?\n"); 
	}
	clerror |= clReleaseCommandQueue(clqueue); 
	if(clerror != CL_SUCCESS){
		clGetCommandQueueInfo(clqueue, CL_QUEUE_REFERENCE_COUNT, sizeof(cl_uint), &references, 0); 
		Error("\tMEMORY LEAK!\n\t%d references to 'clqueue' still exist.\n\tBAILING OUT OF PROGRAM\n", references); 
	}
	clerror |= clReleaseContext(clcontext); 
	if(clerror != CL_SUCCESS){
		clGetContextInfo(clcontext, CL_CONTEXT_REFERENCE_COUNT, sizeof(cl_uint), &references, 0); 
		Error("\tMEMORY LEAK!\n\t%d references to 'clcontext' still exist.\n\tBAILING OUT OF PROGRAM\n", references); 
	}

}
void do_stuff(){  
	int i; 
	local_ws = 512; 
	global_ws = 16384;  
	awesome = MAGIC_SIZE; 
	

		src_a = clCreateBuffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, MEM_SIZE, &src_a_h, &clerror); 
		src_b = clCreateBuffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, MEM_SIZE, &src_b_h, &clerror); 
		res = clCreateBuffer(clcontext, CL_MEM_WRITE_ONLY, MEM_SIZE, &res_h, &clerror); 
	
		magic = clCreateKernel(clprogram, "vector_add_gpu", &clerror); 

		clSetKernelArg(magic, 0, sizeof(cl_mem), &src_a); 
		clSetKernelArg(magic, 1, sizeof(cl_mem), &src_b); 
		clSetKernelArg(magic, 2, sizeof(cl_mem), &res); 
		clSetKernelArg(magic, 3, sizeof(size_t), &awesome); 

		clEnqueueNDRangeKernel(clqueue, magic, 1, NULL, &global_ws, &local_ws, 0, NULL, &clevent);  
				clerror = clWaitForEvents(1, &clevent); 
		if(clerror != CL_SUCCESS){
			Sys_Printf("clWaitForEvents failed.\n"); 
		}
		clGetEventProfilingInfo(clevent, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL); 
		clGetEventProfilingInfo(clevent, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL); 
   
		total = (double)(end - start) * 1.0e-6f; 
		clEnqueueReadBuffer(clqueue, res, CL_TRUE, 0, MEM_SIZE, &res_h, 0, NULL, &clevent); 



		for(stupid = 0; stupid < MAGIC_SIZE; stupid++){
			for(i = 0; i < 4; i++){ 
				Sys_Printf("%f\n", res_h[stupid].s[i]);
			}
		}

		clReleaseKernel(magic); 
}