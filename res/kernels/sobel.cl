__kernel void sobel(__global const uchar* input, __global uchar* output, int width, int height, int mode)
{
    int x = get_global_id(0);
    int y = get_global_id(1);

    int idx = y * width + x;

    if (x == 0 || y == 0 || x == width - 1 || y == height - 1)
    {
        output[y* width + x] = 0;
    }

    int p00 = input[(y - 1) * width + (x - 1)];
    int p01 = input[(y - 1) * width + x];
    int p02 = input[(y - 1) * width + (x + 1)];

    int p10 = input[y * width + (x - 1)];
    int p12 = input[y * width + (x + 1)];

    int p20 = input[(y + 1) * width + (x - 1)];
    int p21 = input[(y + 1) * width + x];
    int p22 = input[(y + 1) * width + (x + 1)];

    int gx = -p00 + p02 - 2 * p10 + 2 * p12 - p20 + p22;
    int gy = -p00 - 2 * p01 - p02 + p20 + 2 * p21 + p22;

    int value;
    if (mode == 0)
    {
        value = abs(gy);
    }
    else if (mode == 1)
    {
        value = abs(gx);
    }    
    else if (mode == 2)
    {
        value = sqrt((float)(gx * gx + gy * gy));
    }
    else
    {
        value = abs(gx) + abs(gy);
    }

    if (value > 255)
    {
        value = 255;
    }

    output[idx] = (uchar)value;
}