#pragma once
#include <mutex>
#include "bookmark_types.h"
#include "bookmark_preferences.h"

#include <vector>
#include <list>
#include <sstream>
#include <iomanip>

namespace dlg {

	class CListControlBookmark;

}

class bookmark_automatic {

public:

	bookmark_automatic() {
		//..
	};

	~bookmark_automatic() {
		//..
	}

	bool checkDummy() {
		return (bool)dummy.get_desc().get_length();
	}

	const bookmark_t getDummy() {
		return dummy;
	}

	bool getDyna() {
		return dummy.isRadio() && dummy.get_dyna();
	}
	void setDyna(bool state) {
		dummy.set_dyna(dummy.isRadio() && state);
	}

	bool checkDummyIsRadio() {
		return (bool)dummy.isRadio();
	}

	bool checkDummyIsRadio(const pfc::string8 path) {
		return (bool)dummy.isRadio(path);
	}

	bool fetchHelloRadioStationName(pfc::string8 &out);

	bool CheckAutoPlaylistFilter();
	bool CheckRadioFilter() { return CheckRadioFilter("", cfg_txt_filter.get_value(), cfg_tf_filter.get_value()); }
	bool CheckRadioFilter(pfc::string8 p_song_desc, const pfc::string8 p_csvfilters, const pfc::string8 p_tf_filter);

	void updateDummyTime();
	void updateDummy(const metadb_handle_ptr p_pmh_now_playing);
#ifdef REC_AUDIO
	bool IsRecording(bool start, pfc::string8 path/*, pfc::string8 artist, pfc::string8 title*/);
	void StartRecording(std::list< dlg::CListControlBookmark*> guiLists, bool start, pfc::string8 path);
#endif
	bool upgradeDummy(const metadb_handle_ptr pmh_now_playing, std::list< dlg::CListControlBookmark*> guiList);

	void ResetRestoredDummy();
	void ResetRestoredDummyTime();
	void SetRestoredDummy(bookmark_t& bm);
	void checkDeletedRestoredDummy(const bit_array& mask, size_t count);

	void resetDummyKeepDyna() {
		dummy.resetDummyKeepDyna();
	}

	void resetDummyAll() {
		m_updatePlaylistLapseStart = DBL_MAX;
		dummy.reset();
	}
	void setDummyTime(double time) { dummy.set_time(time); }

	bool isRestoredDummy(const bookmark_t& bm);
	bool isRestoredRadioDummy(const bookmark_t& bm);

	void delete_item_ui(size_t index, std::list< dlg::CListControlBookmark*> guiLists);
	void refresh_ui(bool bselect, bool bensure_visible, std::list< dlg::CListControlBookmark*> guiLists);

	void Reset_Updating() {
		m_updating = true;
		m_updatePlaylistLapseStart = DBL_MAX;
	}

	void User_Reset_Updating() {
		dummy.need_playlist = true;
		m_user_reset_after_current = true;
		m_updatePlaylistLapseStart = DBL_MAX;
	}

	const bool isUpdating() {
		return m_updating;
	}

	void cancelUpdating() {
		m_updatePlaylistLapseStart = DBL_MAX;
		m_updating = dummy.need_playlist = false;
	}

private:

	bookmark_t dummy;
	bookmark_t restored_dummy;

	bool m_updating = true;
	bool m_user_reset_after_current = false;

	double m_updatePlaylistLapse = 0.0;
	double m_updatePlaylistLapseStart = DBL_MAX;
	titleformat_object::ptr m_ptfo_title = nullptr;

	std::mutex lock_update_time;
};
