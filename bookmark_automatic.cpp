#include "stdafx.h"

#include "SDK/playback_control.h"
#include "SDK/playlist.h"

#include "bookmark_core.h"
#include "bookmark_store.h"
#include "bookmark_automatic.h"
#include "bookmark_list_control.h"
#include "bookmark_preferences.h"

#include "utils.h"
#include "radio_filter_titleformat_hook.h"

using namespace glb;
namespace fltr = filters;

void bookmark_automatic::updateDummyTime() {

	dummy.set_rt_time(playback_control::get()->playback_get_position());

	if (is_cfg_LapseEnabled()) {

		if (m_updatePlaylistLapseStart == DBL_MAX) {
			m_updatePlaylistLapseStart = dummy.get_time();
		}

		m_updatePlaylistLapse = dummy.get_time() - m_updatePlaylistLapseStart;
	}

	if (m_updating || m_user_reset_after_current) {

		bool b_data_srv_available = false;

		pfc::string8 playing_playlist_name;
		size_t playing_playlist_index;

		if (dummy.need_playlist && !dummy.playlist.get_length())
		{
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
				if (is_cfg_LapseEnabled()) {
				//reduced verbose logs
				if (m_updatePlaylistLapse < 3 || (atoi(cfg_lapse.get_value() - m_updatePlaylistLapse < 1 ) {
						FB2K_console_print_v("Track details ready, checking delay... ", dummy.desc);
					}
				}
				else {
					FB2K_console_print_v("Track details done. ", dummy.get_desc());
				}
			}

			if (!m_user_reset_after_current && is_cfg_LapseEnabled()) {

				if(m_updatePlaylistLapse < get_cfg_lapse()) {

					//do not wait for lapse
					if (restored_dummy.path.get_length()) {
						bool bradio_restored = isRestoredRadioDummy(dummy);
						bool bdummy_restored = isRestoredDummy(dummy);
						if (bradio_restored || bdummy_restored) {
							ResetRestoredDummy();
							cancelUpdating();
						}
					}

					//fix empty playlist manually bookmarking before delay expires
					if (dummy.need_playlist) {
						//todo: remove m_updating from updateDummy()
						updateDummy();
						if (bcan_autosave_newtrack) {
							m_updating = true;
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

			m_user_reset_after_current = false;

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
				m_updating = dummy.need_playlist = false;

				//

				return;

				//
			}


		}
		else {

			// loc not available

			// check lapse first, retries may be > LOC_RETRIES for non-playlist items (queued, ...)
			bool blapse_enabled_completed = is_cfg_LapseEnabled() && m_updatePlaylistLapse > get_cfg_lapse();

			if (blapse_enabled_completed || (!is_cfg_LapseEnabled() && dummy.need_loc_retries > LOC_RETRIES)) {

				FB2K_console_print_v(PFC_string_formatter() << "<update time>: " << (blapse_enabled_completed ? "delayed" : "too many retries"));

				if (!dummy.need_playlist) {
					FB2K_console_print_v("delayed, queued, too many retries, details... ", dummy.get_desc());
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
					m_updating = dummy.need_playlist = false;

					//

					return;

					//
				}
			}
			else {

                // retry orphans

                return;

                //
            }
		}

		//req. for no delays, paused bm, auto-create on_exit...
		if (dummy.need_playlist) {
			updateDummy();
		}

		m_updating = dummy.need_playlist = false;
	}
}

bool bookmark_automatic::fetchHelloRadioStationName(pfc::string8& out) {
	if (!checkDummyIsRadio())
	{
		return false;
	}

	if (cfg_autosave_radio_comment.get()) {

		//station
		pfc::string8 station;
		metadb_handle_ptr mhp;

		if (playback_control::get()->get_now_playing(mhp)) {

			metadb_info_container::ptr pmic;
			mhp->get_info_ref(pmic);
			if (pmic.get_ptr()) {
				size_t pos = pmic->info().meta_find("title");
				if (pos != SIZE_MAX) {
					station = pmic->info().meta_get("title", 0);
				}
			}
		}

		if (station.get_length()) {

			out = filters::autoFixEncoding(station.c_str()).c_str();
			return true;

		}
		else {

			if (m_ptfo_title.is_empty()) {
				static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(m_ptfo_title, "%title%");
			}

			pfc::string_formatter title;
			if (playback_control::get()->playback_format_title(NULL, title, m_ptfo_title, NULL, playback_control::display_level_titles)) {
				if (!title.equals("?")) {
					out = title;
					return true;
				}
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

			if (dummy.get_dyna() && !dummy.get_fdn().get_length()) {

				//station
				pfc::string8 station;
				metadb_handle_ptr mhp;

				if (playback_control::get()->get_now_playing(mhp)) {

					//Station

					file_info_impl fi;
					mhp->get_info(fi);
					size_t pos = fi.meta_find("title");
					if (pos != SIZE_MAX) {
						station = fi.meta_get("title",0);
						station = filters::autoFixEncoding(station.c_str()).c_str();
					}
				}

				bool is_scoop = station.toLower().has_prefix("scoop");

				pfc::string8 title;
				titleformat_object::ptr tfo_fdn;
				static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(tfo_fdn, "%title%");
				b_done = playback_control::get()->playback_format_title(NULL, title, tfo_fdn, NULL, playback_control::display_level_all);
				FB2K_console_print_v("Track check-in ", title);

				bool piped = filters::is_dyna_double_pipe(dummy.get_fdn());
				bool custom = false;

				pfc::string8 artist;
				static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(tfo_fdn, "%artist%");
				b_done = playback_control::get()->playback_format_title(NULL, artist, tfo_fdn, NULL, playback_control::display_level_all);

				if (is_scoop) { pfc::swap_t(artist, title); }

				if (!artist.equals("?")) {
					
					if (piped) {
						dummy.set_fdn(PFC_string_formatter() << title << " - " << artist);
					}
					else {
						custom = true;
						if (title.get_length()) {
							dummy.set_fdn(PFC_string_formatter() << artist << "~" << title);
						}
						else {
							FB2K_console_print_v("Track check-in ", dummy.get_fdn(), ", skipping artist ", artist);;
						}
					}
				}
				else {
					dummy.set_fdn(title);
				}

				pfc::string8 album;
				static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(tfo_fdn, "%album%");
				b_done = playback_control::get()->playback_format_title(NULL, album, tfo_fdn, NULL, playback_control::display_level_all);

				if (custom) {
					dummy.set_fdn(PFC_string_formatter() << dummy.get_fdn() << "~" << (album.equals("?") ? "" : album));
				}
				else {
					FB2K_console_print_v("Track check-in ", dummy.get_fdn(), ", skipping album ", album);;
				}

				pfc::string8 year;
				static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(tfo_fdn, "%year%");
				b_done = playback_control::get()->playback_format_title(NULL, year, tfo_fdn, NULL, playback_control::display_level_all);
				if (custom) {
					dummy.set_fdn(PFC_string_formatter() << dummy.get_fdn() << "~" << (year.equals("?") ? "" : year) << "~vbm");
				}
				else {
					FB2K_console_print_v("Track check-in ", dummy.get_fdn(), ", skipping year ", year);
				}
			}
			//fb2k provides hook for new songs
			b_done = playback_control::get()->playback_format_title(NULL, songDesc, desc_format, NULL, playback_control::display_level_all);

			radio_filter_titleformat_hook ra_hook;
			std::vector<pfc::string8>vfilters;

			fltr::get_filters(cfg_txt_filter.get_value(), vfilters);
			ra_hook.setData(dummy.get_fdn(), vfilters);


			fltr::radio_nfo_type rnt;
			fltr::get_radio_nfo(dummy.get_fdn(), rnt);

			size_t pri_pos = fltr::parse_radio_info(rnt, &ra_hook, songDesc, cfg_desc_format.get_value());
			//
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
				guid_playing_playlist = pl_man->playlist_get_guid(index_playlist);
			}
			else {
				//..
			}
		}
		else {

			//todo: radio station without playlist ???
			if (dummy.isRadio(songPath) && !dummy.get_desc().get_length() && !dummy.get_dyna()) {
				b_done = false;
			}
			else {
				b_done = true;
			}
		}

		m_updating = !b_done;
		m_updating &= dummy.need_loc_retries <= LOC_RETRIES;

		dummy.set_time(playback_control_ptr->playback_get_position());
		dummy.path = songPath;

		if (!dummy.isRadio()) {
			dummy.set_desc(songDesc);
		}
		else {

				if (!dummy.get_desc().get_length()) {
					dummy.set_desc(songDesc);
				}
				else {
					if (dummy.get_dyna()) {
						dummy.set_desc(songDesc);
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
		if (!dummy.get_comment().get_length() && fetchHelloRadioStationName(station_name)) {
			dummy.set_comment(station_name);
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

bool bookmark_automatic::CheckRadioFilter(pfc::string8 song_desc, const pfc::string8 p_csvfilters, const pfc::string8 p_tf_filter) {

	bool bres = true;

	radio_filter_titleformat_hook ra_hook;
	std::vector<pfc::string8>vfilters;

	pfc::string8 songDesc = song_desc.get_length() ? song_desc : dummy.get_fdn();

	fltr::get_filters(p_csvfilters, vfilters);
	ra_hook.setData(dummy.get_fdn(), vfilters);

	pfc::string8 flt_in_csv;
	std::vector<pfc::string8>vfields;

	std::pair<size_t, size_t> primary_sig = fltr::get_radio_info_sigfields(songDesc, vfields);

	if (!vfields.size()) vfields.push_back(songDesc);

	std::pair<size_t, size_t> pres = fltr::filters_in_fields(vfields, vfilters);

	if (pres == std::pair(SIZE_MAX, SIZE_MAX)) {
		FB2K_console_print_v("RF allowed");
	}
	else if (pres.first <= fltr::kMinRadioFields) {
		FB2K_console_print_v("RF blocked, skipping bm. Filter: ", vfilters[pres.second], ", primary field: ", vfields[pres.first]);
		//todo: testing TF, should return.
		bres = false;
	}
	else {
		if (pres.second < SIZE_MAX) {
			FB2K_console_print_v("RF allowed, bm sec. passed. Filter: ", vfilters[pres.second], ", field: ", vfields[pres.first]);
		}
		else {
			FB2K_console_print_v("RF allowed, bm passed.");
		}
	}

	FB2K_console_print_v("RF running TF filter...");

	pfc::string_formatter sf_filter;
	pfc::string8 filter_res;

	bool b_done;
	if (vfields.size() > fltr::kMinRadioFields) {
		//todo: it is running get_radio_info_sigfields again
		fltr::radio_nfo_type rnt{ songDesc, primary_sig, vfields };
		size_t ires = fltr::parse_radio_info(rnt, &ra_hook, filter_res, p_tf_filter.c_str());
		b_done = ires != SIZE_MAX;
	}
	else {
		titleformat_object::ptr tfo_filter;
		static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(tfo_filter, p_tf_filter.c_str());
		b_done = playback_control::get()->playback_format_title(&ra_hook, filter_res, tfo_filter, NULL, playback_control::display_level_all);
	}
	if (b_done)
	{
		if (pfc::string_is_numeric(filter_res)) {
			if (atoi(filter_res) != 0) {
				FB2K_console_print_v("RF TF blocked, skipping bm. Num. val > 0 : ", filter_res);
				bres &= false;
			}
			else {
				FB2K_console_print_v("RF TF allowed. Num. val: ", filter_res);
			}
		}
		else {

			pfc::string8 tmpstr;
			if (std::find_if(vfilters.begin(), vfilters.end(), [filter_res](const pfc::string8 s)
				{ return s.equals(filter_res); }) != vfilters.end()) {
				FB2K_console_print_v("RF TF blocked with MATCH in filters, skipping bm. Filter: ", filter_res);
				bres &= false;
			}
		}
	}
	else {
		FB2K_console_print_v("RF TF error running expression.");
	}

	return bres;
}

bool bookmark_automatic::upgradeDummy(std::list< dlg::CListControlBookmark*> guiLists) {

	if (dummy.isRadio()) {

		if (!CheckRadioFilter()) {
			return false;
		}
	}

	if (dummy.need_playlist && dummy.need_loc_retries <= LOC_RETRIES) {
		return false;
	}

	const std::vector<bookmark_t>& masterList = g_store.GetMasterList();

	FB2K_console_print_v("Checking store.");

	size_t old_size = masterList.size();

	if (dummy.get_desc().length() == 0) {
		// nothing to do
		FB2K_console_print_v("Skip save, no dummy description.");
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
			FB2K_console_print_v("Skipping, bm was being restored");
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

		size_t radio_signa_len = SIZE_MAX;

		if (dummy.isRadio()) {
			//todo:
			radio_filter_titleformat_hook ra_hook;
			std::vector<pfc::string8>vfilters;
			fltr::get_filters( cfg_txt_filter.get_value(), vfilters);
			ra_hook.setData(dummy.get_fdn(), vfilters);
			//
			fltr::radio_nfo_type rnt;
			fltr::get_radio_nfo(dummy.get_fdn(), rnt);
			pfc::string8 buff;
			radio_signa_len = fltr::parse_radio_info(rnt, &ra_hook, buff, cfg_desc_format.get_value().c_str());
			dummy.set_desc(buff);
		}

		size_t dup_ndx = SIZE_MAX;

		for (auto rit = std::rbegin(masterList); rit != std::rend(masterList); ++rit) {

			//rev. more renames

			bool brev_start = dummy.get_time() < 2 * KMin_Lapse;
			bool brev_time = abs(rit->get_time() - dummy.get_time()) <= 2 * KMin_Lapse;
			bool brev_path_guid_subsong = rit->path.equals(dummy.path) && pfc::guid_equal(rit->guid_playlist, dummy.guid_playlist);
			brev_path_guid_subsong = brev_path_guid_subsong && rit->subsong == dummy.subsong;
			bool brev_desc_or_radio = !dummy.isRadio() || (rit->get_desc().equals(dummy.get_desc()));

			bool bradio_same_desc_any_time = dummy.isRadio() && brev_path_guid_subsong;

			//sig
			if (radio_signa_len != SIZE_MAX && rit->get_desc().get_length() >= radio_signa_len) {
				bradio_same_desc_any_time &= rit->get_desc().subString(0, radio_signa_len).equals(dummy.get_desc().subString(0, radio_signa_len));
			}
			else {
				bradio_same_desc_any_time &= dummy.isRadio() && brev_path_guid_subsong && rit->get_desc().equals(dummy.get_desc());
			}
			//

			if (dummy.need_playlist || (brev_time && brev_path_guid_subsong && brev_desc_or_radio) || bradio_same_desc_any_time) {

				if (allowed_duplicates && (brev_start)) {

					dup_ndx = std::distance(std::rbegin(masterList), rit);
					dup_ndx = masterList.size() - dup_ndx - 1;

					if (realloc_prev_duplicate) {
						bit_array_bittable changeMask(bit_array_false(), masterList.size());
						changeMask.set(dup_ndx, true);
						g_store.Remove(changeMask);
						FB2K_console_print_v("Deleted duplicated bmookmark: ", dummy.path);
						delete_item_ui(dup_ndx, g_guiLists);
						break;
					}
				}
				else {
					if (!dummy.need_playlist && (brev_time && brev_path_guid_subsong)) {
						FB2K_console_print_v("Skipping duplicated bookmark: ", dummy.path);
					}
					FB2K_console_print_v("Nothing to do.");
					// nothing to do
					return false;
				}
			}
		}

		bool bshutting_down = core_api::is_shutting_down();
		if (bshutting_down && masterList.size()) {
			FB2K_console_print_v("Shutting down.");
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
	else {
		FB2K_console_print_v("Auto-Bookmarking is paused");
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
		(restored_dummy.path.equals(bm.path)) && restored_dummy.get_desc().equals(bm.get_desc())) {
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

void bookmark_automatic::refresh_ui(bool bselect, bool bensure_visible, std::list< dlg::CListControlBookmark*> guiLists) {

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
