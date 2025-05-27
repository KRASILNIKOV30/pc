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
			m_program = cl::Program(m_context, kernelSource, true);
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
			sizeof(T) * v.size(), static_cast<void*>(v.data())
		};
	}

	template <class T>
	cl::Buffer GetOutputBuffer(std::vector<T> const& v)
	{
		return {
			m_context, CL_MEM_WRITE_ONLY,
			sizeof(T) * v.size()
		};
	}

private:
	void InitializeOpenCL()
	{
		m_device = SelectDevice();
		std::cout << "Selected device: " << m_device.getInfo<CL_DEVICE_NAME>() << std::endl;
		m_context = cl::Context(m_device);
		m_queue = cl::CommandQueue(m_context, m_device);
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

		m_queue.enqueueNDRangeKernel(m_kernel, cl::NullRange, args.globalSize, cl::NullRange);
		m_queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0,
			sizeof(float) * result.size(), static_cast<void*>(result.data()));
		m_queue.finish();
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