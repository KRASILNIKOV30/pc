#include "../ChunkVector.h"
#include "../Gzip.h"
#include "../Timer.h"
#include "../WaitChildProcesses.h"
#include <sstream>
#include <filesystem>

struct Args
{
	std::string archiveName;
	int numProcesses;
	std::string outputFolder;
};

Args ParseCommandLine(const int argc, char** argv)
{
	if (argc < 4)
	{
		throw std::invalid_argument("Wrong number of arguments");
	}

	const std::string mode(argv[1]);

	if (mode == "-S")
	{
		std::string archiveName = argv[2];
		std::string outputFolder = argv[3];

		return Args
		{
			.archiveName = archiveName,
			.numProcesses = 0,
			.outputFolder = outputFolder,
		};
	}

	if (mode == "-P")
	{
		int numProcesses = std::stoi(argv[2]);
		std::string archiveName = argv[3];
		std::string outputFolder = argv[4];

		return Args
		{
			.archiveName = archiveName,
			.numProcesses = numProcesses,
			.outputFolder = outputFolder,
		};
	}

	throw std::invalid_argument("Invalid arguments");
}

void UnpackArchive(std::string const& archiveName, std::string const& outputFolder)
{
	std::stringstream command;
	command << "tar -xf " << archiveName << ".tar -C " << outputFolder;
	if (system(command.str().c_str()) != 0)
	{
		throw std::runtime_error("Error extracting tar archive");
	}
}

void ExtractFiles(const Args& mode)
{
	Timer timer(std::cout, "ExtractFiles");
	pid_t pid = getpid();
	std::vector<pid_t> childPids;
	std::vector<std::string> files;

	UnpackArchive(mode.archiveName, mode.outputFolder);

	for (const auto& file : std::filesystem::directory_iterator(mode.outputFolder))
	{
		if (file.path().extension() == ".gz")
		{
			files.push_back(file.path().string());
		}
	}

	const auto chunkSize = std::max<size_t>(files.size() / (mode.numProcesses + 1), 1);
	const auto chunks = ChunkVector(files, chunkSize);

	for (int i = 0; i < chunks.size() - 1 && pid != 0; i++)
	{
		pid = fork();
		childPids.emplace_back(pid);
		if (pid == -1)
		{
			throw std::runtime_error("Error forking process");
		}
		if (pid == 0)
		{
			GzipFiles("-d", chunks.at(i));
			exit(0);
		}
	}

	if (pid != 0)
	{
		GzipFiles("-d", chunks.back());
		WaitChildProcesses(childPids);
		timer.Stop();
	}
}

int main(const int argc, char** argv)
{
	try
	{
		const auto args = ParseCommandLine(argc, argv);
		ExtractFiles(args);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}

