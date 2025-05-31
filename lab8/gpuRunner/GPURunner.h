#pragma once
#include <CL/cl.h>
#include <CL/opencl.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

using ArgValue = std::variant<float, int>;

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
	GPURunner(const char* kernelSource)
		: m_kernelSource(kernelSource)
	{
		InitializeOpenCL();
		// Cleanup();
		BuildProgram();
	}

	template <class T>
	void Run(std::string const& kernelName, KernelArgs const& args, std::vector<T>& result)
	{
		m_kernel = { m_program, kernelName.c_str() };
		return ExecuteKernel<T>(args, result);
	}

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

	void Cleanup()
	{
		try
		{
			if (m_queue())
			{
				m_queue.finish();
			}
			m_kernel = cl::Kernel();
			// clReleaseProgram(m_program.get());
			m_program = cl::Program();
		}
		catch (...)
		{
		}
	}

	~GPURunner()
	{
		// Cleanup();
		std::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);
		for (auto& platform : platforms)
		{
			platform.unloadCompiler();
		}
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

	void BuildProgram()
	{
		cl_int err;
		m_program = cl::Program(m_context, m_kernelSource, true, &err);
		cl_int buildErr = m_program.build("-cl-opt-disable");
		if (buildErr != CL_SUCCESS)
		{
			const auto buildLog = m_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_device);
			std::cerr << "Build log: " << buildErr << std::endl
					  << buildLog << std::endl;
			throw std::runtime_error("OpenCL build failed");
		}
	}

	template <class T>
	void ExecuteKernel(KernelArgs const& args, std::vector<T>& result)
	{
		size_t i = 0;
		for (const auto& value : args.argValues)
		{
			if (holds_alternative<float>(value))
			{
				m_kernel.setArg(i++, get<float>(value));
			}
			else
			{
				m_kernel.setArg(i++, get<int>(value));
			}
		}
		for (const auto& buffer : args.inputBuffers)
		{
			m_kernel.setArg(i++, buffer);
		}

		auto& outputBuffer = args.outputBuffer;
		m_kernel.setArg(i, outputBuffer);

		auto executeErr = m_queue.enqueueNDRangeKernel(m_kernel, cl::NullRange, args.globalSize, cl::NullRange);
		auto readErr = m_queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0,
			sizeof(T) * result.size(), result.data());
		m_queue.finish();

		if (executeErr != CL_SUCCESS)
		{
			std::cerr << "Executing error: " << executeErr << "\n";
		}
		if (readErr != CL_SUCCESS)
		{
			std::cerr << "Error while read buffer: " << readErr << "\n";
		}
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
	const char* m_kernelSource;
	bool m_initialized = false;
};