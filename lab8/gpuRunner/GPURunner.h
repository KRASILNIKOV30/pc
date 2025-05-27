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
	cl::NDRange localSize;
	std::vector<std::vector<float>> inputBuffers;
	std::vector<float> outputBuffer;
	std::vector<ArgValue> argValues;
};

class GPURunner
{
public:
	virtual ~GPURunner() = default;

	void Run(std::string const& kernelName)
	{
		if (!m_initialized)
		{
			InitializeOpenCL();
			const std::string kernelSource = GetKernelSource();
			m_program = cl::Program(m_context, kernelSource, true);
			m_kernel = { m_program, kernelName.c_str() };
		}

		ExecuteKernel();
	}

protected:
	[[nodiscard]] virtual std::string GetKernelSource() const = 0;
	virtual KernelArgs GetKernelArgs() = 0;

private:
	void InitializeOpenCL()
	{
		m_device = SelectDevice();
		std::cout << "Selected device: " << m_device.getInfo<CL_DEVICE_NAME>() << std::endl;
		m_context = cl::Context(m_device);
		m_queue = cl::CommandQueue(m_context, m_device);
		m_initialized = true;
	}

	void ExecuteKernel()
	{
		auto args = GetKernelArgs();
		size_t i = 0;
		for (const auto& [size, value] : args.argValues)
		{
			m_kernel.setArg(i++, size, value);
		}
		for (const auto& buffer : args.inputBuffers)
		{
			cl::Buffer buf(m_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
				sizeof(float) * buffer.size(), (void*)buffer.data());
			m_kernel.setArg(i++, buf);
		}

		auto& result = args.outputBuffer;
		cl::Buffer outputBuffer(m_context, CL_MEM_WRITE_ONLY,
			sizeof(float) * result.size());

		m_queue.enqueueNDRangeKernel(m_kernel, cl::NullRange, args.globalSize, args.localSize);
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