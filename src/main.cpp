#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif
#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

void PrintPlatformData(vector<cl_platform_id> platforms);
string GetAbsoluteFilePath(const char* path);
cl_program CreateProgram(cl_context context, const char* path);
unsigned char* LoadImage(const char* path, int& height, int& width);
void SaveImage(const char* path, int width, int height,vector<unsigned char> output);

int main(int, char**)
{
    int width;
    int height;
    unsigned char* inputImage = LoadImage("lenna.png", height, width);
    if (inputImage == nullptr)
    {
        return 1;
    }
    vector<unsigned char> output(width * height);

    cl_int  err;
    cl_uint platformCount;
    clGetPlatformIDs(0, nullptr, &platformCount);

    vector<cl_platform_id> platforms(platformCount);
    clGetPlatformIDs(platformCount, platforms.data(), nullptr);
    PrintPlatformData(platforms);

    cl_platform_id platform = platforms[0];

    cl_device_id   device;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);

    cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
    if (err != CL_SUCCESS)
    {
        cout << "Failed to create context\n";
        return 1;
    }
    
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);
    if (err != CL_SUCCESS)
    {
        cout << "Failed to create queue\n";
        return 1;
    }
    
    cl_program program = CreateProgram(context, "invert.cl");
    
    if (program == nullptr)
    {
        return 1;
    }
    
    err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    
    if (err != CL_SUCCESS)
    {
        size_t logSize;
         clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
        vector<char> log(logSize);
    
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, log.data(), nullptr);
    
        cout << log.data() << endl;
    }
    
    cl_kernel kernel = clCreateKernel(program, "invert", &err);
    
    cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, width * height, inputImage, &err);
    cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, width * height, nullptr, &err);
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputBuffer);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &outputBuffer);

    size_t globalSize = static_cast<size_t>(width * height);
    
    err = clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &globalSize, nullptr, 0, nullptr, nullptr);
    
    clFinish(queue);
    
    clEnqueueReadBuffer(queue, outputBuffer, CL_TRUE, 0, width * height, output.data(), 0, nullptr, nullptr);
    
    SaveImage("output1.png", width, height, output);

    stbi_image_free(inputImage);
	return 0;
}

cl_program CreateProgram(cl_context context, const char* path)
{
    string dir = "res/kernels/";
    string dirPath = dir + path;
    string absolutePath = GetAbsoluteFilePath(dirPath.c_str());
    const char* filePath = absolutePath.c_str();
    // 1. retrieve the vertex/fragment source code from filePath
    string clCode;
    ifstream clFile;
    // ensure ifstream objects can throw exceptions:
    clFile.exceptions(ifstream::failbit | ifstream::badbit);
    try
    {
        // open files
        clFile.open(filePath);
        std::stringstream cShaderStream;
        // read file's buffer contents into streams
        cShaderStream << clFile.rdbuf();
        // close file handlers
        clFile.close();
        // convert stream into string
        clCode = cShaderStream.str();
    }
    catch (ifstream::failure e)
    {
        cout << "ERROR::KERNEL::FILE_NOT_SUCCESSFULLY_READ\n";
        cout << "Absolute path to kernel: " << absolutePath << endl;
        return nullptr;
    }
    const char* code = clCode.c_str();

    cl_int err;
    cl_program program = clCreateProgramWithSource(context, 1, &code, nullptr, &err);

    if (err != CL_SUCCESS)
    {
        cout << "Program not created";
        return nullptr;
    }

    return program;
}

unsigned char* LoadImage(const char* path, int& height, int& width)
{
    string dir = "res/images/";
    string dirPath = dir + path;
    string absolutePath = GetAbsoluteFilePath(dirPath.c_str());
    const char* filePath = absolutePath.c_str();
    
    width = 0;
    height = 0;
    int channels = 0;

    unsigned char* img = stbi_load(filePath, &width, &height, &channels, 1);

    if (!img)
    {
        cout << "Failed to load image\n";
        cout << "Absolute Image Path: " << absolutePath << endl;
        return nullptr;
    }

    cout << "Image loaded successfully\n";
    cout << "Width: " << width << "\n";
    cout << "Height: " << height << "\n";
    cout << "Channels: " << channels << "\n";

    cout << "First pixel value: " << (int)img[0] << "\n";

    return img;
    // stbi_image_free(img);
}

void SaveImage(const char* path, int width, int height, vector<unsigned char> output)
{
    string dir = "res/images/";
    string dirPath = dir + path;
    string absolutePath = GetAbsoluteFilePath(dirPath.c_str());
    const char* filePath = absolutePath.c_str();

    stbi_write_png(filePath, width, height, 1, output.data(), width);
}

string GetAbsoluteFilePath(const char* path)
{
#ifdef __APPLE__
    // string relative = "Documents/Studia/PKG/GPGPU/";
    // string fullPath = relative + path;
    string fullPath = path;
    cout << "full path: " << fullPath << endl;
#else
    string relative = "../";
    string fullPath = relative + path;
#endif
    string absolutePath = filesystem::absolute(fullPath).string();

    return absolutePath;
}

void PrintPlatformData(vector<cl_platform_id> platforms)
{
    for (cl_uint p = 0; p < platforms.size(); ++p)
    {
        char platformName[1024];
        clGetPlatformInfo(platforms[p], CL_PLATFORM_NAME, sizeof(platformName), platformName, nullptr);

        cout << "Platform: " << platformName << "\n";

        cl_uint deviceCount = 0;
        clGetDeviceIDs(platforms[p], CL_DEVICE_TYPE_ALL, 0, nullptr, &deviceCount);

        vector<cl_device_id> devices(deviceCount);

        clGetDeviceIDs(platforms[p], CL_DEVICE_TYPE_ALL, deviceCount, devices.data(), nullptr);

        for (cl_uint d = 0; d < deviceCount; ++d)
        {
            char deviceName[1024];
            char vendor[1024];

            cl_uint computeUnits;
            size_t maxWorkGroupSize;

            clGetDeviceInfo(devices[d], CL_DEVICE_NAME, sizeof(deviceName), deviceName, nullptr);
            clGetDeviceInfo(devices[d], CL_DEVICE_VENDOR, sizeof(vendor), vendor, nullptr);
            clGetDeviceInfo(devices[d], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(computeUnits), &computeUnits, nullptr);
            clGetDeviceInfo(devices[d], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxWorkGroupSize), &maxWorkGroupSize, nullptr);

            cout << "  Device: " << deviceName << "\n";
            cout << "  Vendor: " << vendor << "\n";
            cout << "  Compute Units: " << computeUnits << "\n";
            cout << "  Max Work Group Size: " << maxWorkGroupSize << "\n\n";
        }
    }
}
