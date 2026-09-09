#pragma once

#include "bookmark_preferences.h"
#include "bookmark_types.h"
#include "bookmark_persistence.h"
#include <vector>

class bookmark_store {

public:
	bookmark_store();
	~bookmark_store();

	const std::vector<bookmark_t>& GetMasterList() {

		std::lock_guard<std::mutex> guard(m_store_lock);

		return m_masterList;
	}

	void SetMasterList(std::vector<bookmark_t> v) {
		//eg. reordering

		std::lock_guard<std::mutex> guard(m_store_lock);

		m_is_dirty = true;
		m_masterList = std::move(v);
	}

	size_t Size() {

		std::lock_guard<std::mutex> guard(m_store_lock);

		return m_masterList.size();
	};

	bool Initialize() { 

		m_is_dirty = false;
		m_persist.readDataFileJSON(m_masterList); /*todo*/return true;

	}

	const bookmark_t _getItem(size_t pos) {

		return m_masterList.at(pos);

	}
	const bookmark_t GetItem(size_t pos) {

		std::lock_guard<std::mutex> guard(m_store_lock);

		return _getItem(pos);
	}

	void _setItem(size_t pos, bookmark_t rec) {
		m_masterList[pos] = rec;
	}
	void SetItem(size_t pos, bookmark_t rec) {

		std::lock_guard<std::mutex> guard(m_store_lock);

		m_is_dirty = true;
		_setItem(pos, rec);
	}

	void _addItem(const bookmark_t rec) {
		m_masterList.emplace_back(rec);
	}
	void AddItem(const bookmark_t rec) {

		std::lock_guard<std::mutex> guard(m_store_lock);

		m_is_dirty = true;
		_addItem(rec);
	}

	void _reorder(const pfc::array_t<t_size> p_order, t_size p_count) {
		pfc::reorder_t(m_masterList, p_order.get_ptr(), p_count);
	}
	void Reorder(const pfc::array_t<t_size> p_order, t_size p_count) {

		std::lock_guard<std::mutex> guard(m_store_lock);

		m_is_dirty = true;
		_reorder(p_order, p_count);
	}
	void _write() {

		auto write_callback = [this](std::lock_guard<std::mutex>* p_guard) {

			std::lock_guard<std::mutex>* guard = p_guard;

			m_is_dirty = false;
		};

		std::lock_guard<std::mutex> guard(m_store_lock);

		m_persist.writeDataFile(m_masterList, write_callback, &guard);

		return;
	}

	void Write(bool thread_pool = true) {

		{
			std::lock_guard<std::mutex> guard(m_store_lock);

			if (!m_is_dirty) {
				FB2K_console_print_v("Saving... nothing to do.");
				return;
			}
		}

		//not thread safe
		setlocale(LC_ALL, ".UTF8");
		//

		if (thread_pool) {

			if (!is_cfg_Instant_Write()) {
				FB2K_console_print_v("Saving later.");
				return;
			}

			//thread pool, m_is_dirty set by callback
			_write();
		}
		else {
			//app close blocker splitTask

			auto work = [this] {
				try {

					std::lock_guard<std::mutex> guard(m_store_lock);

					this->m_persist.writeDataFileJSON(this->m_masterList);
					this->m_is_dirty = false;
					FB2K_console_print_v("Saved.");
				}
				catch (std::exception const& /*e*/) {
					//..
				}
			};
			fb2k::splitTask(work);
			return;
			}
	}

	void _remove(const bit_array& p_mask) {
		pfc::remove_mask_t(m_masterList, p_mask);
	}
	void Remove(const bit_array_bittable p_mask) {

		std::lock_guard<std::mutex> guard(m_store_lock);

		m_is_dirty = true;
		_remove(p_mask);
	}

	void _clear() { m_masterList.clear(); }
	void Clear() {

		std::lock_guard<std::mutex> guard(m_store_lock);

		m_is_dirty = true;
		_clear();
	}

private:

	inline static std::mutex m_store_lock;

	std::vector<bookmark_t> m_masterList;
	bookmark_persistence m_persist;
	bool m_is_dirty = false;
};


namespace std {
	inline void swap(bookmark_t& a, bookmark_t& b)
	{
		a.swap(b);
	}
}
