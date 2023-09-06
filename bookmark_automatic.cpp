#include "stdafx.h"

#include "SDK/playback_control.h"
#include "SDK/playlist.h"

#include "bookmark_automatic.h"
#include "bookmark_list_control.h"
#include "bookmark_preferences.h"

void bookmark_automatic::updateDummyTime() {

	dummy.time = playback_control::get()->playback_get_position();

	if (m_updatePlaylist) {
		m_updatePlaylist = false;

		pfc::string_fixed_t<80> playing_pl_name;
		size_t index_playlist;
		size_t index_item;
		auto playlist_manager_ptr = playlist_manager_v5::get();

		if (cfg_verbose) {
			if (playlist_manager_ptr->get_playing_item_location(&index_playlist, &index_item)) {
				FB2K_console_print_v("AutoBookmark: dummy update: Playlist index is ", index_playlist);
				if (playlist_manager_ptr->playlist_get_name(index_playlist, playing_pl_name))
					FB2K_console_print_v("AutoBookmark: dummy update: Playlist name is ", playing_pl_name);
			}
			else {
				FB2K_console_print_v("AutoBookmark: dummy update: couldn't find playlist index");
			}
		}

		//Set the flag back to true if either operation fails
		m_updatePlaylist |= !playlist_manager_ptr->get_playing_item_location(&index_playlist, &index_item);
		m_updatePlaylist |= !playlist_manager_ptr->playlist_get_name(index_playlist, playing_pl_name);

		dummy.playlist = playing_pl_name.c_str();

		if (core_version_info_v2::get()->test_version(2, 0, 0, 0)) {
			dummy.guid_playlist = playlist_manager_ptr->playlist_get_guid(index_playlist);
		}

		if (!m_updatePlaylist) {
            //update last item
            //todo: delay insertion - add instead of update.
            bit_array_bittable changeMask(bit_array_false(), glb::g_primaryGuiList->GetItemCount());
            changeMask.set(glb::g_primaryGuiList->GetItemCount() - 1, true);
            auto& last = glb::g_masterList.at(glb::g_primaryGuiList->GetItemCount() - 1);
            last.playlist = dummy.playlist;
            last.guid_playlist = dummy.guid_playlist;

            for (auto gui : glb::g_guiLists) {
                gui->ReloadItems(changeMask);
            }
            //
        }

		gimme_time(dummy.date);
	}
}

//Fully update the dummy.
void bookmark_automatic::updateDummy() {

	metadb_handle_ptr dbHandle_item;
	auto playback_control_ptr = playback_control::get();

	if (playback_control_ptr->get_now_playing(dbHandle_item)) {
		pfc::string_formatter songDesc;
		titleformat_object::ptr desc_format;
		static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(desc_format, cfg_desc_format.get_value().c_str());

		if (!dbHandle_item->format_title(NULL, songDesc, desc_format, NULL)) {
			songDesc << "Could not generate Description.";
		}

		m_updatePlaylist = true;	//We can't read the PlName right after the track was changed
		dummy.playlist = "";
		dummy.guid_playlist = pfc::guid_null;

		pfc::string8 songPath = dbHandle_item->get_path();

		//TODO: graceful failure?!

		dummy.time = playback_control_ptr->playback_get_position();
		dummy.desc = songDesc;
		dummy.path = songPath;
		dummy.subsong = dbHandle_item->get_subsong_index();
		gimme_time(dummy.date);
	}
	else {
		dummy.time = 0.0;
		dummy.desc = "";
		dummy.path = "";
		dummy.subsong = 0;
	}
}

bool bookmark_automatic::upgradeDummy(std::vector<bookmark_t>& masterList, std::list< dlg::CListControlBookmark*> guiLists) {

	if (dummy.desc.length() == 0) {
		// nothing to do
		return false;
	//TODO: what if there is no valid song name?

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

	//track ending ?
	auto track_length = track_bm->get_length();
	bool bsamepath = pfc::string8(track_bm->get_path()).equals(dummy.path);
	if (bsamepath) {
		if (dummy.time + 3 >= track_bm->get_length()) {
			// nothing to do
			return false;
		}
	}
	else {
		updateDummy();
	}

	if (cfg_autosave_filter_newtrack.get()) {
		//active filter

		FB2K_console_print_v("AutoBookmark: The dummie's playlist is called ", dummy.playlist.c_str());

		//Obtain individual names in the filter
		std::vector<std::string> allowedPlaylists;
		std::stringstream ss(cfg_autosave_newtrack_playlists.get_value().c_str());
		std::string token;
		while (std::getline(ss, token, ',')) {
			allowedPlaylists.push_back(token);
		}

		//replace chars in playlist names
		pfc::string8 dummyPlaylist = dummy.playlist.c_str();
		dummyPlaylist.replace_char(',', '.');

		auto find_it = std::find(allowedPlaylists.begin(), allowedPlaylists.end(), dummyPlaylist.c_str());
		if (find_it == allowedPlaylists.end()) {
			
			FB2K_console_print_v("...no match.");
			// nothing to do
			return false;

		}
	}

	FB2K_console_print_v("...matches.");

	double timeFuzz = 1.0;

	if (is_cfg_Bookmarking()) {

		for (std::vector<bookmark_t>::iterator it = masterList.begin(); it != masterList.end(); ++it) {
			if ((abs(it->time - dummy.time) <= timeFuzz) &&
				(it->path.equals(dummy.path)) &&
				(it->playlist.equals(dummy.playlist)) &&
				(pfc::guid_equal(it->guid_playlist, dummy.guid_playlist))) {

				// nothing to do
				return false;
			}
		}

		FB2K_console_print_v("AutoBookmark: storing");

		bookmark_t lastmark;
		if (masterList.size()) {
			lastmark = masterList.at(masterList.size() - 1);
		}

		if (lastmark.path.get_length() && lastmark.path.equals(dummy.path)) {
			//if quitting dummy is the last item
			masterList.at(masterList.size() - 1) = dummy;
		}
		else {
			//The filter was either disabled or matched the current playlist, continue:
			bookmark_t copy = bookmark_t(dummy);
			masterList.emplace_back(std::move(copy));	//store the mark
		}

		//Update all gui lists
		if (!core_api::is_shutting_down()) {
			for (auto it = guiLists.begin(); it != guiLists.end(); ++it) {
				dlg::CListControlBookmark* lc = *it;
				lc->OnItemsInserted(masterList.size(), 1, true);				
			}
		}
	}
	return true;
}
