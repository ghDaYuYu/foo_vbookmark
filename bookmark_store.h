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
		return m_masterList;
	}

	void SetMasterList(std::vector<bookmark_t> v) {
		m_is_dirty = true;
		m_masterList = std::move(v);
	}

	size_t Size() {
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
		return _getItem(pos);
	}

	void _setItem(size_t pos, bookmark_t rec) {
		m_masterList[pos] = rec;
	}
	void SetItem(size_t pos, bookmark_t rec) {
		m_is_dirty = true;
		_setItem(pos, rec);
	}

	void _addItem(const bookmark_t rec) {
		m_masterList.emplace_back(rec);
	}
	void AddItem(const bookmark_t rec) {
		m_is_dirty = true;
		_addItem(rec);
	}

	void _reorder(const pfc::array_t<t_size> p_order, t_size p_count) {
		pfc::reorder_t(m_masterList, p_order.get_ptr(), p_count);
	}
	void Reorder(const pfc::array_t<t_size> p_order, t_size p_count) {
		m_is_dirty = true;
		_reorder(p_order, p_count);
	}
	void _write() {

		auto write_callback = [this] {
			m_is_dirty = false;
		};

		m_persist.writeDataFile(m_masterList, write_callback);

		return;
	}

	void Write(bool thread_pool = true) {

		if (!m_is_dirty) {
			return;
		}

		if (thread_pool) {

			if (!is_cfg_Instant_Write()) {
				return;
			}

			//thread pool, m_is_dirty set by callback
			_write();
		}
		else {
			//app close blocker splitTask

			auto work = [this] {
				try {
					this->m_persist.writeDataFileJSON(this->m_masterList);
					this->m_is_dirty = false;
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
		m_is_dirty = true;
		_remove(p_mask);
	}

	void _clear() { m_masterList.clear(); }
	void Clear() {
		m_is_dirty = true;
		_clear();
	}

private:
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
