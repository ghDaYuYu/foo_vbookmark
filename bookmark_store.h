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
		//eg. reordering

		m_is_dirty = true;
		m_masterList = std::move(v);
	}

	size_t Size() {
		//todo: NoRefreshScope
		if (m_nofresh) {
			return 0;
		}

		return m_masterList.size();
	};

	//todo: remove callbacks
	bool Initialize(bool ordered, std::function<void()> p_callback) {

		m_is_dirty = false;
		return m_persist.readDataFileJSON(m_masterList, ordered, p_callback);
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
	void AddItem(const bookmark_t rec, std::function<void()> p_add_bookmark_callback) {

		ThreadUtils::cmdThread cmdThStore;
		cmdThStore.add([this, rec, p_add_bookmark_callback]() {

			size_t c = 0;

			while (c < 10) {

				try {
					{
						std::lock_guard<std::mutex> guard(get_lock());
						m_is_dirty = true;
						_addItem(rec);
					}
					p_add_bookmark_callback();
					break;
				}
				catch (...) {
					Sleep(1000);
					c++;
				}
			}

			});
	}

	void _addItems(const std::vector<bookmark_t> vrec) {
		m_masterList.insert(m_masterList.end(), vrec.begin(), vrec.end());
	}
	void AddItems(const std::vector<bookmark_t> vrec, std::function<void()> p_add_bookmark_callback) {
		ThreadUtils::cmdThread cmdThStore;
		cmdThStore.add([this, vrec, p_add_bookmark_callback]() {

			size_t c = 0;

			while (c < 10) {

				try {
					{
						std::lock_guard<std::mutex> guard(get_lock());
						m_is_dirty = true;
						_addItems(vrec);
					}
					p_add_bookmark_callback();
					break;
				}
				catch (...) {
					Sleep(1000);
					c++;
				}
				}
			});
		}
	
	void _reorder(const pfc::array_t<t_size> p_order, t_size p_count) {
		pfc::reorder_t(m_masterList, p_order.get_ptr(), p_count);
	}
	void Reorder(const pfc::array_t<t_size> p_order, t_size p_count) {
		size_t c = 0;
		while (c < 10) {
			try {
				std::lock_guard<std::mutex> guard(get_lock());
				m_is_dirty = true;
				_reorder(p_order, p_count);
				break;
			}
			catch (...) {
				Sleep(1000);
				c++;
			}
		}
		
	}
	void _write() {

		auto write_callback = [this]() {

			m_is_dirty = false;

		};

		try {
			std::lock_guard<std::mutex> guard(m_store_lock);
			m_persist.writeDataFile(m_masterList, write_callback);
		}
		catch (...) {
			FB2K_console_print_v("Skipping writting to file busy");
			return;
		}
		return;
	}

	void Write(bool thread_pool = true);

	void _remove(const bit_array& p_mask) {
		pfc::remove_mask_t(m_masterList, p_mask);
	}
	void Remove(const bit_array_bittable p_mask, std::function<void()> p_remove_callback) {

		fb2k::splitTask([this, p_mask, p_remove_callback]() {

			size_t c = 0;

			while (c < 10) {
				try {
					{
						std::lock_guard<std::mutex> guard(get_lock));
						m_is_dirty = true;
						_remove(p_mask);
					}
					p_remove_callback();
					break;
				}
				catch (...) {
					Sleep(1000);
					c++;
				}
			}
			});
	}

	void _clear() { m_masterList.clear(); }
	void Clear(std::function<void()> p_callback) {

		fb2k::splitTask([this, p_callback]() {

			size_t c = 0;

			while (c < 10) {
				try {
					std::lock_guard<std::mutex> guard(get_lock());
					m_is_dirty = true;
					_clear();
					p_callback();
					break;
				}
				catch (...) {
					Sleep(1000);
					c++;
				}
			}
			});
	}

	inline static void set_no_refresh(bool st) {
		m_nofresh = st;
	}
	inline static bool get_no_refresh() {
		return m_nofresh;
	}

	std::vector<bookmark_t> Discard_Bookmarks(std::vector<bookmark_t> master_list);

	inline static std::mutex& get_lock() {
		return bookmark_store::m_store_lock;
	}

private:

	inline static std::mutex m_store_lock;
	inline static bool m_nofresh = false;

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
