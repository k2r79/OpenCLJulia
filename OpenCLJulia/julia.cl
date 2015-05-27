#if defined(cl_khr_fp64)  // Khronos extension available?
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#elif defined(cl_amd_fp64)  // AMD extension available?
#pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif

typedef struct cuComplex {
    float x;
    float y;
} cuComplex;

static cuComplex createComplex(float jx, float jy) {
    cuComplex z;
    z.x = jx;
    z.y = jy;
    
    return z;
}

static cuComplex add(cuComplex c1, cuComplex c2) {
    cuComplex z;
    z.x = c1.x + c2.x;
    z.y = c1.y + c2.y;
    
    return z;
}

static cuComplex multiply(cuComplex c1, cuComplex c2) {
    cuComplex z;
    z.x = c1.x * c2.x - c1.y * c2.y;
    z.y = c1.y * c2.x + c1.x * c2.y;
    
    return z;
}

static float magnitude2(cuComplex z) {
    return z.x * z.x + z.y * z.y;
}

static int isJulia(int x, int y, int dimension) {
    const float scale = 1.50;
    float jx = scale * (float)(dimension / 2 - x) / (dimension / 2);
    float jy = scale * (float)(dimension / 2 - y) / (dimension / 2);
    
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

__kernel void julia(__global unsigned char *ptr)
{
    int x = get_global_id(0);
    int y = get_global_id(1);
    
    const int dimension = 1000;
    int index = (y + x * dimension) * 4;
    
    ptr[index] =  0;
    ptr[index + 1] =  isJulia(x, y, dimension) * 255;
    ptr[index + 2] =  0;
    ptr[index + 3] =  255;
}