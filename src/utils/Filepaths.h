#include <filesystem>
#include <string>

class Filepaths
{
private:
	Filepaths() = delete;

public:
	static const std::string workingDir;


	static const std::string roms;


	/*****************************************************************//*!
	\brief
		Adds the working directory to the target string.
	\param targetString
		The target string.
	*//******************************************************************/
	static void AddWorkingDirectoryTo(std::string* targetString);
	/*****************************************************************//*!
	\brief
		Adds the working directory to the target string.
	\param targetString
		The target string.
	\return
		The modified target string.
	*//******************************************************************/
	static std::string AddWorkingDirectoryTo(const std::string& targetString);

	/*****************************************************************//*!
	\brief
		Trims the working directory from the target string.
	\param targetString
		The target string.
	*//******************************************************************/
	static void TrimWorkingDirectoryFrom(std::string* targetString);
	/*****************************************************************//*!
	\brief
		Trims the working directory from the target string.
	\param targetString
		The target string.
	\return
		The modified target string.
	*//******************************************************************/
	static std::string TrimWorkingDirectoryFrom(const std::string& targetString);

	/*****************************************************************//*!
	\brief
		Converts an absolute filepath to a relative filepath.
	\param path
		The absolute filepath.
	\return
		The relative filepath.
	*//******************************************************************/
	static std::string MakeRelativeToWorkingDir(const std::filesystem::path& path);

	/*****************************************************************//*!
	\brief
		Checks if a directory points to a location within the working directory.
	\param path
		The filepath.
	\return
		True if path points to a location within the working directory.
	*//******************************************************************/
	static bool IsWithinWorkingDir(const std::filesystem::path& path);

};