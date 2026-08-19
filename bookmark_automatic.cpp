#include "stdafx.h"

#include "SDK/playback_control.h"
#include "SDK/playlist.h"

//update playlist
#include "bookmark_core.h"
#include "bookmark_store.h"
#include "bookmark_automatic.h"
#include "bookmark_list_control.h"
#include "bookmark_preferences.h"

using namespace glb;

void bookmark_automatic::updateDummyTime() {

	dummy.set_rt_time(playback_control::get()->playback_get_position());

	if (is_cfg_LapseEnabled()) {

		if (m_updatePlaylistLapseStart == DBL_MAX) {
			m_updatePlaylistLapseStart = dummy.get_time();
		}

		m_updatePlaylistLapse = dummy.get_time() - m_updatePlaylistLapseStart;
	}

	if (m_updating) {

		bool b_data_srv_available = false;

		pfc::string8 playing_playlist_name;
		size_t playing_playlist_index;

		if (dummy.need_playlist && !dummy.playlist.get_length())
		{
			//rev: no playlist items querying location

			size_t index_item;

			bool bItemLoc = playlist_manager_v5::get()->get_playing_item_location(&playing_playlist_index, &index_item);
			bool bPlaylist = false;

			if (bItemLoc) {
				FB2K_console_print_v("<update time> - Item location playlist index: ", playing_playlist_index);

				bPlaylist = playlist_manager_v5::get()->playlist_get_name(playing_playlist_index, playing_playlist_name);

				if (bPlaylist) {
					FB2K_console_print_v("<update time> - Item location playlist name: ", playing_playlist_name);
				}
			}
			else {
				FB2K_console_print_v("<update time> - Unknown item location ", dummy.path);

				//increase retries
				++dummy.need_loc_retries;
			}

			b_data_srv_available = bItemLoc && bPlaylist;

		}
		else {
			//..
		}

		bool bcan_autosave_newtrack = cfg_autosave_newtrack.get();
		bcan_autosave_newtrack &= cfg_autosave_radio_newtrack.get() || !dummy.isRadio();

		bool b_write_store = false;

		if (b_data_srv_available || !dummy.need_playlist) {

			// loc available

			if (!dummy.need_playlist) {
				FB2K_console_print_v("Track details available, checking delay... ", dummy.desc);
			}

			if (is_cfg_LapseEnabled()) {

				if(m_updatePlaylistLapse < get_cfg_lapse()) {

					//do not wait for lapse
					if (restored_dummy.path.get_length()) {
						bool bradio_restored = isRestoredRadioDummy(dummy);
						bool bdummy_restored = isRestoredDummy(dummy);
						if (bradio_restored || bdummy_restored) {
							ResetRestoredDummy();
						}
					}

					//
					return;
					//

				}
				else {
					if (m_updatePlaylistLapseStart != DBL_MAX) {
						dummy.set_rt_time(m_updatePlaylistLapseStart);
					}
				}
			}

			if (bcan_autosave_newtrack) {

				// AUTO - CREATE

				if (b_data_srv_available && dummy.need_playlist) {

					updateDummy();
					if (is_cfg_LapseEnabled()) {

						if (m_updatePlaylistLapseStart != DBL_MAX) {
							dummy.set_rt_time(m_updatePlaylistLapseStart);
						}
					}
				}

				bool bres = upgradeDummy(g_guiLists);
				m_updatePlaylistLapseStart = DBL_MAX;
			}
			else if (cfg_autosave_on_quit.get()) {
				updateDummy();
			}

			m_updating = dummy.need_playlist = false;
		}
		else {

			// loc not available

			// check lapse first, retries may be > LOC_RETRIES for non-playlist items (queued, ...)
			bool blapse_enabled_completed = is_cfg_LapseEnabled() && m_updatePlaylistLapse > get_cfg_lapse();

			if (blapse_enabled_completed || (!is_cfg_LapseEnabled() && dummy.need_loc_retries > LOC_RETRIES)) {

				FB2K_console_print_v(PFC_string_formatter() << "<update time>: " << (blapse_enabled_completed ? "delayed" : "too many retries"));

				if (!dummy.need_playlist) {
					FB2K_console_print_v("delayed, queued, too many retries, details... ", dummy.desc);
				}

				if (bcan_autosave_newtrack) {

					if (is_cfg_LapseEnabled()) {

						if (m_updatePlaylistLapseStart != DBL_MAX) {
							dummy.set_rt_time(m_updatePlaylistLapseStart);
						}
					}

					// AUTO - CREATE
					bool bres = upgradeDummy(g_guiLists);
					m_updatePlaylistLapseStart = DBL_MAX;
				}
				else if (cfg_autosave_on_quit.get()) {
					updateDummy();
				}

				m_updating = dummy.need_playlist = false;
			}
		}
	}
}

bool bookmark_automatic::fetchHelloRadioStationName(pfc::string8& out) {
	if (!checkDummyIsRadio())
	{
		return false;
	}

	if (cfg_autosave_radio_comment.get()) {
		pfc::string_formatter sfTitle;
		if (m_pttf_title.is_empty()) {
			static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(m_pttf_title, "%title%");
		}
		if (playback_control::get()->playback_format_title(NULL, sfTitle, m_pttf_title, NULL, playback_control::display_level_titles)) {
			if (sfTitle.get_length() > 1) {
				out = sfTitle;
				return true;
			}
		}
	}
	return false;
}

//Full update
void bookmark_automatic::updateDummy() {

	metadb_handle_ptr dbHandle_item;
	auto playback_control_ptr = playback_control::get();

	if (playback_control_ptr->get_now_playing(dbHandle_item)) {

		bool b_done = false;

		pfc::string_formatter songDesc;
		titleformat_object::ptr desc_format;
		static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(desc_format, cfg_desc_format.get_value().c_str());

		pfc::string8 songPath = dbHandle_item->get_path();

		if (checkDummyIsRadio(songPath)) {
			b_done = playback_control::get()->playback_format_title(NULL, songDesc, desc_format, NULL, playback_control::display_level_all);
		}
		else {
			b_done = dbHandle_item->format_title(NULL, songDesc, desc_format, NULL);
		}

		if (!b_done) {
			songDesc << "Could not generate description.";
		}

		pfc::string8 playing_playlist_name;
		GUID guid_playing_playlist = pfc::guid_null;

		auto pl_man = playlist_manager_v5::get();

		bool playlist_available = false;

		size_t index_item;
		size_t index_playlist;
		auto check_available = pl_man->get_playing_item_location(&index_playlist, &index_item);
		if (check_available) {
			bool bres = pl_man->get_playing_item_location(&index_playlist, &index_item);
			playlist_available = pl_man->playlist_find_item(index_playlist, dbHandle_item, index_item);
		}

		if (!check_available || playlist_available) {

			b_done &= pl_man->get_playing_item_location(&index_playlist, &index_item);
			b_done &= pl_man->playlist_get_name(index_playlist, playing_playlist_name);

			if (b_done) {
				FB2K_console_print_v("<update dummy> - Item location: ", playing_playlist_name);
				guid_playing_playlist = pl_man->playlist_get_guid(index_playlist);
			}
			else {
				FB2K_console_print_v("<update dummy> - Fetching item location...");
			}
		}
		else {

			//todo: radio station without playlist ???
			if (dummy.isRadio(songPath) && !dummy.desc.get_length() && !dummy.dyna) {
				b_done = false;
			}
			else {
				b_done = true;
			}
		}

		m_updating = !b_done;
		m_updating &= dummy.need_loc_retries <= LOC_RETRIES;

		//TODO: graceful failure?!

		dummy.set_time(playback_control_ptr->playback_get_position());
		dummy.path = songPath;

		if (!dummy.isRadio()) {
			dummy.desc = songDesc;
		}
		else {

				if (!dummy.desc.get_length()) {
					dummy.desc = songDesc;
				}
				else {
					if (dummy.dyna) {
						dummy.desc = songDesc;
					}
				}
		}

		dummy.subsong = dbHandle_item->get_subsong_index();
		if (playlist_available) {
			dummy.playlist = playing_playlist_name;
			dummy.guid_playlist = guid_playing_playlist;
			dummy.need_playlist = !playlist_available;
		}
		gimme_date(dummy);

		//dyna
		pfc::string8 station_name;
		if (!dummy.comment.get_length() && fetchHelloRadioStationName(station_name)) {
			dummy.comment = station_name;
		}

		if (m_updating) {
			++dummy.need_loc_retries;
		}
	}
	else {
		if (!core_api::is_shutting_down()) {
			dummy.reset();
		}
	}
}

bool bookmark_automatic::CheckAutoPlaylistFilter() {
	if (cfg_autosave_filter_newtrack.get()) {
		//Obtain individual names in the filter
		std::vector<std::string> allowedPlaylists;
		std::stringstream ss(cfg_autosave_newtrack_playlists.get_value().c_str());
		std::string token;
		while (std::getline(ss, token, ',')) {
			allowedPlaylists.push_back(token);
		}

		//replace chars in the current playlist names
		pfc::string8 dummyPlaylist = dummy.playlist.c_str();
		dummyPlaylist.replace_char(',', '.');

		auto find_it = std::find(allowedPlaylists.begin(), allowedPlaylists.end(), dummyPlaylist.c_str());
		if (find_it == allowedPlaylists.end()) {
			// nothing to do
			return false;
		}
	}

	return true;
}
bool bookmark_automatic::upgradeDummy(std::list< dlg::CListControlBookmark*> guiLists) {

	const std::vector<bookmark_t>& masterList = g_store.GetMasterList();

	if (dummy.need_playlist) {
		return false;
	}

	size_t old_size = masterList.size();

	if (dummy.desc.length() == 0) {
		// nothing to do
		return false;
	}

	metadb_handle_ptr track_bm;
	metadb_handle_ptr track_current;

	//not playing on_quit - is shutting down
	bool bnowPlaying = playback_control_v3::get()->get_now_playing(track_current);
	if (bnowPlaying) {
		track_bm = track_current;
	}
	else {
		auto metadb_ptr = metadb::get();
		track_bm = metadb_ptr->handle_create(dummy.path.c_str(), dummy.subsong);
	}

	auto track_subsong = bnowPlaying ? track_current->get_subsong_index() : track_bm->get_subsong_index();
	auto track_length = track_bm->get_length();
	bool bsamepath = pfc::string8(track_bm->get_path()).equals(dummy.path);
	bsamepath &= track_subsong == dummy.subsong;

	//todo: unify with local files which are rejected by duplicated criteria
	bool bradio_restored = isRestoredRadioDummy(dummy);
	bool bdummy_restored = isRestoredDummy(dummy);

	if (!core_api::is_shutting_down()) {
		if (bsamepath && (bradio_restored || bdummy_restored)) {
			ResetRestoredDummy();
			return false;
		}
		else {
			//..
		}
	}

	if (!CheckAutoPlaylistFilter()) {
		FB2K_console_print_v("Filter is active and did not match, do not store a bookmark.");
		//
		return false;
		//
	}


	if (is_cfg_Bookmarking()) {

		bool allowed_duplicates = is_cfg_Dupli_Enabled();
		bool realloc_prev_duplicate = is_cfg_Dupli_Remove_Prev();

		size_t dup_ndx = SIZE_MAX;

		for (auto rit = std::rbegin(masterList); rit != std::rend(masterList); ++rit) {
			//rev. renames
			bool brevstart = dummy.get_time() < 2 * KMin_Lapse;
			bool brevtime = abs(rit->get_time() - dummy.get_time()) <= 2 * KMin_Lapse;
			bool brevpath = rit->path.equals(dummy.path) && pfc::guid_equal(rit->guid_playlist, dummy.guid_playlist);
			brevpath = brevpath && rit->subsong == dummy.subsong;
			bool brevradio = !dummy.isRadio() || (rit->desc.equals(dummy.desc));

			if (dummy.need_playlist || (brevtime && brevpath && brevradio)) {

				if (allowed_duplicates && brevstart) {
					dup_ndx = std::distance(std::rbegin(masterList), rit);
					dup_ndx = masterList.size() - dup_ndx - 1;
					if (realloc_prev_duplicate) {
						bit_array_bittable changeMask(bit_array_false(), masterList.size());
						changeMask.set(dup_ndx, true);
						g_store.Remove(changeMask);
						delete_item_ui(dup_ndx, g_guiLists);
						break;
					}
				}
				else {
					if (!dummy.need_playlist && (brevtime && brevpath)) {
						FB2K_console_print_e("Skipping duplicated bookmark: ", dummy.path);
					}
					// nothing to do
					return false;
				}
			}
		}

		bool bshooting_down = core_api::is_shutting_down();
		if (bshooting_down && masterList.size()) {
			//..
		}
		else {

			g_store.AddItem(std::move(bookmark_t(dummy)));
			g_store.Write();

			FB2K_console_print_v("Dummy stored");
		}

		//UI update
		if (!bshooting_down) {
			bool bscroll_list = cfg_autosave_focus_newtrack.get();
			refresh_ui(bscroll_list, bscroll_list, guiLists);
		}
	}
	return g_store.Size() != old_size;
}

void bookmark_automatic::ResetRestoredDummy() {
	restored_dummy.reset();
}

void bookmark_automatic::ResetRestoredDummyTime() {
	restored_dummy.set_time(0.0);
}

void bookmark_automatic::SetRestoredDummy(bookmark_t& bm) {
	restored_dummy = bookmark_t(bm);
}

bool bookmark_automatic::isRestoredDummy(const bookmark_t& bm) {
	if (bm.isRadio()) return false;
	if (restored_dummy.get_time() || pfc::guid_equal(restored_dummy.guid_playlist, bm.guid_playlist) &&
		(restored_dummy.path.equals(bm.path)) && abs(restored_dummy.get_time() - bm.get_time()) <= 3) {
		return true;
	}
	return false;
}

bool bookmark_automatic::isRestoredRadioDummy(const bookmark_t& bm) {
	if (!bm.isRadio()) return false;
	if (pfc::guid_equal(restored_dummy.guid_playlist, bm.guid_playlist) &&
		(restored_dummy.path.equals(bm.path)) && restored_dummy.desc.equals(bm.desc)) {
		return true;
	}
	return false;
}

void bookmark_automatic::checkDeletedRestoredDummy(const bit_array& mask, size_t count) {

	auto f = mask.find_first(true, 0, count);
	if (f >= count) {
		ResetRestoredDummy();
	}
	else {
		for (auto n = f; n < count; n = mask.find_next(true, n, count)) {
			bookmark_t tmpbm = g_store.GetItem(n);
			if (isRestoredDummy(tmpbm)) {
				ResetRestoredDummy();
				break;
			}
		}
	}
}

void bookmark_automatic::refresh_ui(bool bselect, bool bensure_visible/*, const std::vector<bookmark_t>& masterList*/, std::list< dlg::CListControlBookmark*> guiLists) {
	for (auto it = guiLists.begin(); it != guiLists.end(); ++it) {

		dlg::CListControlBookmark* lc = *it;

		if (lc->TableEdit_IsActive()) {
			lc->TableEdit_Abort(false);
		}

		size_t item = lc->GetItemCount() - 1;
		if (lc->GetSortOrder()) {
			item = 0;
		}

		if (bselect) {
			lc->SelectNone();
		}
		lc->OnItemsInserted(item, 1, bselect);
		if (bselect || bensure_visible) {
			lc->EnsureItemVisible(item, false);
		}
		if (bselect) {
			lc->SetFocusItem(item);
		}
	}
}

void bookmark_automatic::delete_item_ui(size_t index, std::list< dlg::CListControlBookmark*> guiLists) {

	for (auto it = guiLists.begin(); it != guiLists.end(); ++it) {

		bit_array_bittable changeMask(bit_array_false(), g_primaryGuiList->GetItemCount());

		size_t new_pos = index;
		if ((*it)->GetSortOrder()) {
			if (index >= (*it)->GetItemCount()) {
				index = 0;
			}
			else {
				index = (*it)->GetItemCount() - index;
			}
		}
		auto isel = (*it)->GetSingleSel();
		if ((*it)->GetSingleSel() == index) {
			if ((*it)->TableEdit_IsActive()) {
				(*it)->TableEdit_Abort(false);
			}
		}
		(*it)->SelectNone();
		(*it)->OnItemRemoved(index);
	}
}
