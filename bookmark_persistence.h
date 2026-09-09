#pragma once
#include <filesystem>
#include "helpers/CmdThread.h"
#include "bookmark_types.h"
#include <vector>

inline ThreadUtils::cmdThread cmdThFile;

class bookmark_persistence {

public:

	bookmark_persistence();
	~bookmark_persistence();

	void writeDataFile(const std::vector<bookmark_t>& masterList,
		td::function<void(std::lock_guard<std::mutex>* p_guard)> sf_write_callback, std::lock_guard<std::mutex>*p_guard);
	//Stores the content of g_masterList in a persistent file
	bool readDataFileJSON(std::vector<bookmark_t>& masterList);
	//Stores the contents of g_masterList in a persistent file
	bool writeDataFileJSON(const std::vector<bookmark_t>& masterList);

private:

	static void replaceMasterList(std::vector<bookmark_t>& newContent, std::vector<bookmark_t>& masterList);

	std::filesystem::path bookmark_persistence::genFilePath();
};
