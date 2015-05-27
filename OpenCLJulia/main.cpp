#ifdef __APPLE__
    #include "OpenCL/opencl.h"
#else
    #include "CL/cl.h"
#endif

#include "cl.hpp"
#include <iostream>
#include "cpu_bitmap.h"

#define MAX_SOURCE_SIZE (0x100000)
#define INPUT_WIDTH (4)
#define INPUT_HEIGHT (4)

#define DIM 1000

typedef struct cuComplex {
    float x;
    float y;
} cuComplex;

cuComplex createComplex(float jx, float jy) {
    return { jx, jy };
}

cuComplex add(cuComplex c1, cuComplex c2) {
    return { c1.x + c2.x, c1.y + c2.y };
}

cuComplex multiply(cuComplex c1, cuComplex c2) {
    return { c1.x * c2.x - c1.y * c2.y, c1.y * c2.x + c1.x * c2.y };
}

float magnitude2(cuComplex z) {
    return z.x * z.x + z.y * z.y;
}

int julia(int x, int y) {
    const float scale = 1.5;
    float jx = scale * (float)(DIM / 2 - x) / (DIM / 2);
    float jy = scale * (float)(DIM / 2 - y) / (DIM / 2);
    
    cuComplex c = createComplex(-0.8, 0.156);
    cuComplex a = createComplex(jx, jy);
    
    int i = 0;
    for (i = 0; i<200; i++) {
        a = add(multiply(a, a), c);
        if (magnitude2(a) > 1000)
            return 0;
    }
    
    return 1;
}


int main(int argc, const char * argv[]) {
    CPUBitmap bitmap(DIM, DIM);
    unsigned char *ptr = bitmap.get_ptr();
    
    for (int y = 0; y < DIM; y++) {
        for (int x = 0; x < DIM; x++) {
            ptr[(x * 4) + (y * 4 * DIM)] =  julia(x, y) * 255;
            ptr[(x * 4) + (y * 4 * DIM) + 1] =  0;
            ptr[(x * 4) + (y * 4 * DIM) + 2] =  0;
            ptr[(x * 4) + (y * 4 * DIM) + 3] =  255;
        }
    }
    
    bitmap.display_and_exit();
    
    return 0;
}

//int main(int argc, const char * argv[]) {
//    // VARIABLES
//    const size_t INPUT_ROW_SIZE = INPUT_WIDTH * INPUT_HEIGHT;
//    cl_int error;
//    cl_platform_id platform;
//    cl_device_id device;
//    cl_context context;
//    cl_command_queue queue;
//    cl_program program;
//    cl_kernel kernel;
//    
//    // Initialize the input array (Linear)
//    int **pixels = new int*[3];
//    pixels[0] = new int[INPUT_ROW_SIZE];
//    pixels[1] = new int[INPUT_ROW_SIZE];
//    pixels[2] = new int[INPUT_ROW_SIZE];
//    
//    // Set the RGB values of the input array
//    for (int i = 0; i < INPUT_ROW_SIZE; i++) {
//        pixels[0][i] = 130;
//        pixels[1][i] = 55;
//        pixels[2][i] = 200;
//        
//        printf("%d : %d, %d, %d\n", i, pixels[0][i], pixels[1][i], pixels[2][i]);
//    }
//    
//    // Initializa the ouput array
//    int *output_pixels = (int*) malloc(INPUT_ROW_SIZE * sizeof(int));
//    
//    // Select the platform
//    error = clGetPlatformIDs(1, &platform, NULL);
//    
//    // Select the device
//    error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
//    
//    // Create the context
//    context = clCreateContext(0, 1, &device, NULL, NULL, &error);
//    
//    // Create the command queue
//    queue = clCreateCommandQueue(context, device, 0, &error);
//    
//    // Load the source code containing the kernel
//    FILE *fp = fopen("/Users/Vincent/Workspace/OpenCL/OpenCLGrayscale/OpenCLGrayscale/grayscale.cl", "r");
//    if (!fp) {
//        fprintf(stderr, "Failed to load kernel.\n");
//        exit(1);
//    }
//    char *source_str = (char*) malloc(MAX_SOURCE_SIZE);
//    size_t source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
//    fclose(fp);
//    
//    // Create the grayscale program
//    program = clCreateProgramWithSource(context, 1, (const char **) &source_str, (const size_t *) &source_size, &error);
//    
//    // Build the grayscale kernel program
//    error = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
//    
//    // Create the OpenCL kernel
//    kernel = clCreateKernel(program, "grayscale", &error);
//
//    // Create the input buffers
//    cl_mem r_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, INPUT_ROW_SIZE * sizeof(int), NULL, NULL);
//    cl_mem g_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, INPUT_ROW_SIZE * sizeof(int), NULL, NULL);
//    cl_mem b_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, INPUT_ROW_SIZE * sizeof(int), NULL, NULL);
//    
//    // Create the output buffer
//    cl_mem output_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, INPUT_ROW_SIZE * sizeof(int), NULL, NULL);
//    
//    // Write the data to the input buffers
//    error = clEnqueueWriteBuffer(queue, r_buffer, CL_TRUE, 0, INPUT_ROW_SIZE * sizeof(int), pixels[0], 0, NULL, NULL);
//    error = clEnqueueWriteBuffer(queue, g_buffer, CL_TRUE, 0, INPUT_ROW_SIZE * sizeof(int), pixels[1], 0, NULL, NULL);
//    error = clEnqueueWriteBuffer(queue, b_buffer, CL_TRUE, 0, INPUT_ROW_SIZE * sizeof(int), pixels[2], 0, NULL, NULL);
//    
//    // Set the kernel's parameters
//    error = clSetKernelArg(kernel, 0, sizeof(cl_mem), &r_buffer);
//    error = clSetKernelArg(kernel, 1, sizeof(cl_mem), &g_buffer);
//    error = clSetKernelArg(kernel, 2, sizeof(cl_mem), &b_buffer);
//    error = clSetKernelArg(kernel, 3, sizeof(cl_mem), &output_buffer);
//    
//    // Execute the OpenCL kernel
//    size_t global_item_size = INPUT_ROW_SIZE;
//    size_t local_item_size = 1;
//    error = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
//    
//    // Wait for the command queue to be finished
//    error = clFinish(queue);
//    
//    // Read the device's buffer
//    error = clEnqueueReadBuffer(queue, output_buffer, CL_TRUE, 0, INPUT_ROW_SIZE * sizeof(int), output_pixels, 0, NULL, NULL);
//    
//    // Check the output values
//    for (int i = 0; i < INPUT_ROW_SIZE; i++) {
//        printf("%d : %d\n", i, output_pixels[i]);
//    }
//    
//    // Free the resources
//    error = clReleaseKernel(kernel);
//    error = clReleaseProgram(program);
//    error = clReleaseMemObject(output_buffer);
//    error = clReleaseCommandQueue(queue);
//    error = clReleaseContext(context);
//    
//    return 0;
//}
