#pragma once
#include <filesystem>
#include "helpers/CmdThread.h"
#include "bookmark_types.h"
#include <vector>

inline ThreadUtils::cmdThread cmdThFile;

class bookmark_persistence {
private:
	//..
public:
	bookmark_persistence();
	~bookmark_persistence();

	void writeDataFile(const std::vector<bookmark_t>& masterList, std::function<void() > sf_write_callback);
	//Stores the contents of g_masterList in a persistent file
	bool writeDataFileJSON(const std::vector<bookmark_t>& masterList);

	//Replaces the contents of g_masterList with the contents of the persistent file
	bool readDataFileJSON(std::vector<bookmark_t>& masterList);

private:
	static void replaceMasterList(std::vector<bookmark_t>& newContent, std::vector<bookmark_t>& masterList);

	std::filesystem::path bookmark_persistence::genFilePath();
};
