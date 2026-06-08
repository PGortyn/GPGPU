__kernel void invert(__global const uchar* input, __global uchar* output, int width, int height, int direction)
{
    int x = get_global_id(0);
    int y = get_global_id(1);
    
    int idx = y * width + x;

    output[idx] = 255 - input[idx];
}