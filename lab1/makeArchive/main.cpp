#include "../ChunkVector.h"
#include "../Gzip.h"
#include "../Timer.h"
#include "../WaitChildProcesses.h"
#include <sstream>
#include <gsl/gsl>

struct Args
{
	std::string archiveName;
	int numProcesses;
	std::vector<std::string> files;
};

Args ParseCommandLine(const int argc, char** argv)
{
	if (argc < 4)
	{
		throw std::invalid_argument("Wrong number of arguments");
	}

	std::vector<std::string> files;

	const std::string mode(argv[1]);

	if (mode == "-S")
	{
		std::string archiveName = argv[2];
		for (int i = 3; i < argc; i++)
		{
			files.emplace_back(argv[i]);
		}

		return Args
		{
			.archiveName = archiveName,
			.numProcesses = 0,
			.files = files,
		};
	}

	if (mode == "-P")
	{
		int numProcesses = std::stoi(argv[2]);
		std::string archiveName = argv[3];
		for (int i = 4; i < argc; i++)
		{
			files.emplace_back(argv[i]);
		}

		return Args
		{
			.archiveName = archiveName,
			.numProcesses = numProcesses,
			.files = files,
		};
	}

	throw std::invalid_argument("Invalid arguments");
}

void ArchiveFiles(std::string const& archiveName, std::vector<std::string> const& files)
{
	std::stringstream command;
	command << "tar -cf " << archiveName + ".tar";
	for (const auto& file : files)
	{
		command << " " << file << ".gz";
	}
	if (system(command.str().c_str()) != 0)
	{
		throw std::runtime_error("Error generating tar archive");
	}
}

void DeleteFiles(std::vector<std::string> const& files)
{
	for (const auto& file : files)
	{
		std::remove(file.c_str());
	}
}

void MakeArchive(const Args& args)
{
	Timer timer(std::cout, "MakeArchive");
	pid_t pid = getpid();
	std::vector<pid_t> childPids;
	auto finalizer = gsl::finally([&] {
		if (pid != 0)
		{
			WaitChildProcesses(childPids);
		}
	});
	const auto chunkSize = std::max<size_t>(args.files.size() / (args.numProcesses + 1), 1);
	const auto chunks = ChunkVector(args.files, chunkSize);
	for (int i = 0; i < chunks.size() - 1 && pid != 0; i++)
	{
		pid = fork();
		childPids.emplace_back(pid);
		if (pid == -1)
		{
			// Если выбросится исключение, то кто-то должен дождаться завершения дочерних процессов (Исправлено)
			throw std::runtime_error("Error forking process");
		}
		if (pid == 0)
		{
			GzipFiles("-k", chunks.at(i));
		}
	}

	if (pid != 0)
	{
		GzipFiles("-k", chunks.back());
		WaitChildProcesses(childPids);

		ArchiveFiles(args.archiveName, args.files);
		timer.Stop();

		std::vector<std::string> zippedFiles;
		// В другое место и захватывать по ссылке (Исправлено)
		auto deleteFilesFinalizer = gsl::finally([&] {
			// Удалить файлы в finalizer(исправлено)
			DeleteFiles(zippedFiles);
			DeleteFiles({ args.archiveName + ".tar" });
		});

		zippedFiles.reserve(args.files.size());
		for (const auto& file : args.files)
		{
			zippedFiles.emplace_back(file + ".gz");
		}

	}
}

int main(const int argc, char** argv)
{
	try
	{
		auto args = ParseCommandLine(argc, argv);
		for (int i = 0; i <= 20; ++i)
		{
			args.numProcesses = i;
			MakeArchive(args);
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}

