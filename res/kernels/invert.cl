__kernel void invert(__global const uchar* input, __global uchar* output)
{
    int id = get_global_id(0);

    output[id] = 255 - input[id];
}