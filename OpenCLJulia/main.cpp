#ifdef __APPLE__
    #include "OpenCL/opencl.h"
#else
    #include "CL/cl.h"
#endif

#include "cl.hpp"
#include <iostream>
#include "cpu_bitmap.h"

#define MAX_SOURCE_SIZE (0x100000)
#define DIM 1000

int main(int argc, const char * argv[]) {
    // VARIABLES
    const size_t INPUT_ROW_SIZE = DIM * DIM;
    cl_int error;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    
    // Create the output image
    CPUBitmap bitmap(DIM, DIM);
    unsigned char *ptr = bitmap.get_ptr();
    
    // Select the platform
    error = clGetPlatformIDs(1, &platform, NULL);
    
    // Select the device
    error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    
    // Create the context
    context = clCreateContext(0, 1, &device, NULL, NULL, &error);
    
    // Create the command queue
    queue = clCreateCommandQueue(context, device, 0, &error);
    
    // Load the source code containing the kernel
    FILE *fp = fopen("/Users/Vincent/Workspace/OpenCL/OpenCLJulia/OpenCLJulia/julia.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    char *source_str = (char*) malloc(MAX_SOURCE_SIZE);
    size_t source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);
    
    // Create the grayscale program
    program = clCreateProgramWithSource(context, 1, (const char **) &source_str, (const size_t *) &source_size, &error);
    
    // Build the grayscale kernel program
    error = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    
    // Debug kernel errors
    if (error == CL_BUILD_PROGRAM_FAILURE) {
        // Determine the size of the log
        size_t log_size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        
        // Allocate memory for the log
        char *log = (char *) malloc(log_size);
        
        // Get the log
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        
        // Print the log
        printf("%s\n", log);
    }
    
    // Create the OpenCL kernel
    kernel = clCreateKernel(program, "julia", &error);

    // Create the buffer
    cl_mem buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, INPUT_ROW_SIZE * sizeof(unsigned char) * 4, NULL, NULL);
    
    // Write the data to the buffer
    error = clEnqueueWriteBuffer(queue, buffer, CL_TRUE, 0, INPUT_ROW_SIZE * sizeof(unsigned char) * 4, ptr, 0, NULL, NULL);
    
    // Set the kernel's parameters
    error = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer);
    
    // Execute the OpenCL kernel
    size_t local_work_size[2] = { 4, 1 };
    size_t global_work_size[2] = { DIM * 4, DIM };
    error = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_work_size, local_work_size, 0, NULL, NULL);
    
    // Wait for the command queue to be finished
    error = clFinish(queue);
    
    // Read the device's buffer
    error = clEnqueueReadBuffer(queue, buffer, CL_TRUE, 0, INPUT_ROW_SIZE * sizeof(unsigned char) * 4, ptr, 0, NULL, NULL);
    
    
    // Display the ouput image
    bitmap.display_and_exit();
    
    // Free the resources
    error = clReleaseKernel(kernel);
    error = clReleaseProgram(program);
    error = clReleaseMemObject(buffer);
    error = clReleaseCommandQueue(queue);
    error = clReleaseContext(context);
    
    return 0;
}
