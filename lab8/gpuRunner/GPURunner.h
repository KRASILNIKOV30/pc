#pragma once
#include <CL/cl.h>
#include <CL/opencl.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

struct ArgValue
{
	size_t size;
	void* value;
};

struct KernelArgs
{
	cl::NDRange globalSize;
	std::vector<cl::Buffer> inputBuffers;
	cl::Buffer outputBuffer;
	std::vector<ArgValue> argValues;
};

class GPURunner
{
public:
	virtual ~GPURunner() = default;

	template <class T>
	void Run(std::string const& kernelName, std::vector<T>& result)
	{
		if (!m_initialized)
		{
			InitializeOpenCL();
			const char* kernelSource = GetKernelSource();
			cl_int err;
			m_program = cl::Program(m_context, kernelSource, true, &err);
			std::cout << "Context: " << (m_context() != nullptr);
			cl_int buildErr = m_program.build();
			size_t availableMem = m_device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() / 1024 / 1024;
			std::cout << "availableMem: " << availableMem << std::endl;
			if (buildErr != CL_SUCCESS)
			{
				const auto buildLog = m_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_device);
				std::cerr << "Build log: " << buildErr << std::endl
						  << buildLog << std::endl;
				throw std::runtime_error("OpenCL build failed");
			}
			m_kernel = { m_program, kernelName.c_str() };
		}

		ExecuteKernel<T>(result);
	}

protected:
	[[nodiscard]] virtual const char* GetKernelSource() const = 0;
	virtual KernelArgs& GetKernelArgs() = 0;

	template <class T>
	cl::Buffer GetInputBuffer(std::vector<T>& v)
	{
		return {
			m_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			sizeof(T) * v.size(), v.data()
		};
	}

	template <class T>
	cl::Buffer GetOutputBuffer(std::vector<T>& v)
	{
		return {
			m_context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
			sizeof(T) * v.size(), v.data()
		};
	}

private:
	void InitializeOpenCL()
	{
		m_device = SelectDevice();
		std::cout << "Selected device: " << m_device.getInfo<CL_DEVICE_NAME>() << std::endl;
		cl_int err;
		m_context = cl::Context(m_device, nullptr, nullptr, nullptr, &err);
		m_queue = cl::CommandQueue(m_context, m_device, 0, &err);
		m_initialized = true;
	}

	template <class T>
	void ExecuteKernel(std::vector<T>& result)
	{
		auto& args = GetKernelArgs();
		size_t i = 0;
		for (const auto& [size, value] : args.argValues)
		{
			m_kernel.setArg(i++, size, value);
		}
		for (const auto& buffer : args.inputBuffers)
		{
			m_kernel.setArg(i++, buffer);
		}

		auto& outputBuffer = args.outputBuffer;
		m_kernel.setArg(i, outputBuffer);

		auto err = m_queue.enqueueNDRangeKernel(m_kernel, cl::NullRange, args.globalSize, cl::NullRange);
		m_queue.finish();
		if (err != CL_SUCCESS)
		{
			//	std::cerr << "Executing error: " << err << "\n";
		}
		m_queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0,
			sizeof(T) * result.size(), result.data());
		m_queue.finish();

		// std::cout << result[100].s[0] << result[100].s[1] << result[100].s[2] << std::endl;
	}

	static cl::Device SelectDevice()
	{
		std::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);
		if (platforms.empty())
		{
			throw std::runtime_error("No OpenCL platforms found");
		}

		for (const auto& platform : platforms)
		{
			std::vector<cl::Device> devices;
			platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
			if (!devices.empty())
			{
				return devices.front();
			}
		}
		throw std::runtime_error("No GPU devices found");
	}

private:
	cl::Device m_device;
	cl::Context m_context;
	cl::CommandQueue m_queue;
	cl::Program m_program;
	cl::Kernel m_kernel;
	bool m_initialized = false;
};