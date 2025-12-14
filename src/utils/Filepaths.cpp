#include "Filepaths.h"

#ifdef _DEBUG
const std::string Filepaths::workingDir = "../../..";
#else
const std::string Filepaths::workingDir = ".";
#endif


const std::string Filepaths::roms = Filepaths::workingDir + "/roms";





void Filepaths::AddWorkingDirectoryTo(std::string* targetString)
{
	targetString->insert(0, workingDir);
}

std::string Filepaths::AddWorkingDirectoryTo(const std::string& targetString)
{
	return workingDir + targetString;
}

void Filepaths::TrimWorkingDirectoryFrom(std::string* targetString)
{
	if (targetString->rfind(workingDir, 0) == 0)
		targetString->erase(0, workingDir.size());
}

std::string Filepaths::TrimWorkingDirectoryFrom(const std::string& targetString)
{
	if (!(targetString.rfind(workingDir, 0) == 0))
		return targetString;
	return targetString.substr(workingDir.size());
}

std::string Filepaths::MakeRelativeToWorkingDir(const std::filesystem::path& path)
{
	return '/' + std::filesystem::relative(path, workingDir).string();
}

bool Filepaths::IsWithinWorkingDir(const std::filesystem::path& path)
{
	std::filesystem::path absolutePath = std::filesystem::absolute(path);
	std::filesystem::path absoluteWorkingDir = std::filesystem::absolute(workingDir);

	auto [begin, end] = std::mismatch(
		absoluteWorkingDir.begin(), absoluteWorkingDir.end(),
		absolutePath.begin(), absolutePath.end()
	);

	return begin == absoluteWorkingDir.end();
}