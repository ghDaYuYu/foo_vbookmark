#pragma once

#include <vector>
#include <list>
#include "bookmark_types.h"

class bookmark_worker
{
public:
	bookmark_worker();
	~bookmark_worker();

	static void store(const bookmark_t bookmark, std::function<void()>add_bookmark_callback, bool exact_time = false);
	static void store(const std::vector<bookmark_t> vbookmark, std::function<void()>add_bookmark_callback, bool exact_time = false);
	static void restore(size_t index);
};
