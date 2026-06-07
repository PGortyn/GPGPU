#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif
#include <iostream>
#include <vector>

using namespace std;

int main(int, char**)
{
	cl_uint platformCount;
	clGetPlatformIDs(0, nullptr, &platformCount);

	vector<cl_platform_id> platforms(platformCount);
	clGetPlatformIDs(platformCount, platforms.data(), nullptr);

	for (cl_uint p = 0; p < platformCount; ++p)
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
    int input;
    cin >> input;
	return 0;
}
