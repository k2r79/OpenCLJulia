__kernel void grayscale(__global int *r, __global int *g, __global int *b, __global int *output)
{
    unsigned int i = get_global_id(0);

    output[i] = 0.21 * r[i] + 0.72 * g[i] + 0.07 * b[i];
}