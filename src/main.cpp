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

using namespace std;

const char* source = R"(
__kernel void addOne(__global int* data)
{
    int id = get_global_id(0);
    data[id] += 1;
}
)";

void PrintPlatformData(vector<cl_platform_id> platforms);
cl_program CreateProgram(cl_context context, const char* path);

int main(int, char**)
{
	cl_uint platformCount;
	clGetPlatformIDs(0, nullptr, &platformCount);

	vector<cl_platform_id> platforms(platformCount);
	clGetPlatformIDs(platformCount, platforms.data(), nullptr);
	PrintPlatformData(platforms);

    cl_platform_id platformID = platforms[0];

    cl_device_id deviceID;
    clGetDeviceIDs(platformID, CL_DEVICE_TYPE_GPU, 1, &deviceID, nullptr);

    cl_int err;
    cl_context context = clCreateContext(nullptr, 1, &deviceID, nullptr, nullptr, &err);
    if (err != CL_SUCCESS)
    {
        cout << "Failed to create context\n";
        return 1;
    }

    cl_command_queue queue = clCreateCommandQueue(context, deviceID, 0, &err);

    
    cl_program program = CreateProgram(context, "test.cl");

    if (program == nullptr)
    {
        return 1;
    }

    err = clBuildProgram(program, 1, &deviceID, nullptr, nullptr, nullptr);

    if (err != CL_SUCCESS)
    {
        size_t logSize;
         clGetProgramBuildInfo(program, deviceID, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
        vector<char> log(logSize);

        clGetProgramBuildInfo(program, deviceID, CL_PROGRAM_BUILD_LOG, logSize, log.data(), nullptr);

        cout << log.data() << endl;
    }

    cl_kernel kernel = clCreateKernel(program, "addOne", &err);

    vector<int> values = { 1, 2, 3, 4, 5 };

    cl_mem buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * values.size(), values.data(), &err);
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer);

    size_t globalSize = values.size();

    err = clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &globalSize, nullptr, 0, nullptr, nullptr);

    clFinish(queue);

    clEnqueueReadBuffer(queue, buffer, CL_TRUE, 0, sizeof(int) * values.size(), values.data(), 0, nullptr, nullptr);

    for (int value : values)
    {
        cout << value << " ";
    }
    cout << endl;
    
    // int input;
    // cin >> input;
	return 0;
}

cl_program CreateProgram(cl_context context, const char* path)
{
    filesystem::path p = path;
    cout << "Absolute path: " << filesystem::absolute(p) << endl;
    string relative = "../";
    string dir = "res/kernels/";
    string fullPath = relative + dir + path;
    string ageihniojk = filesystem::absolute(fullPath).string();
    const char* filePath = ageihniojk.c_str();
    // 1. retrieve the vertex/fragment source code from filePath
    std::string clCode;
    std::ifstream clFile;
    // ensure ifstream objects can throw exceptions:
    clFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
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
    catch (std::ifstream::failure e)
    {
       cout << "ERROR::KERNEL::FILE_NOT_SUCCESSFULLY_READ\n";
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

void PrintPlatformData(vector<cl_platform_id> platforms)
{
    for (cl_uint p = 0; p < platforms.size(); ++p)
    {
        char platformName[1024];
        clGetPlatformInfo(
            platforms[p],
            CL_PLATFORM_NAME,
            sizeof(platformName),
            platformName,
            nullptr);

        std::cout << "Platform: "
                  << platformName
                  << "\n";

        cl_uint deviceCount = 0;
        clGetDeviceIDs(
            platforms[p],
            CL_DEVICE_TYPE_ALL,
            0,
            nullptr,
            &deviceCount);

        std::vector<cl_device_id> devices(deviceCount);

        clGetDeviceIDs(
            platforms[p],
            CL_DEVICE_TYPE_ALL,
            deviceCount,
            devices.data(),
            nullptr);

        for (cl_uint d = 0; d < deviceCount; ++d)
        {
            char deviceName[1024];
            char vendor[1024];

            cl_uint computeUnits;
            size_t maxWorkGroupSize;

            clGetDeviceInfo(
                devices[d],
                CL_DEVICE_NAME,
                sizeof(deviceName),
                deviceName,
                nullptr);

            clGetDeviceInfo(
                devices[d],
                CL_DEVICE_VENDOR,
                sizeof(vendor),
                vendor,
                nullptr);

            clGetDeviceInfo(
                devices[d],
                CL_DEVICE_MAX_COMPUTE_UNITS,
                sizeof(computeUnits),
                &computeUnits,
                nullptr);

            clGetDeviceInfo(
                devices[d],
                CL_DEVICE_MAX_WORK_GROUP_SIZE,
                sizeof(maxWorkGroupSize),
                &maxWorkGroupSize,
                nullptr);

            std::cout << "  Device: "
                      << deviceName
                      << "\n";

            std::cout << "  Vendor: "
                      << vendor
                      << "\n";

            std::cout << "  Compute Units: "
                      << computeUnits
                      << "\n";

            std::cout << "  Max Work Group Size: "
                      << maxWorkGroupSize
                      << "\n\n";
        }
    }
}
