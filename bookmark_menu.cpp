#include "stdafx.h"

static const GUID guid_vbookmark_main_menu_group_id = { 0x4da5c373, 0x8c39, 0x49bd, { 0xaf, 0xe4, 0xe3, 0x4c, 0xb0, 0xd9, 0x43, 0x67 } };

static const GUID guid_storeBookmark = { 0x5ae6fa28, 0xe10, 0x4970, { 0xb3, 0x9, 0x8d, 0x37, 0xf0, 0x4a, 0xba, 0x4b } };

// {5EEEF30B-647C-4341-B4E6-8DA88CE326CD}
static const GUID guid_storeBookmarkSelected = { 0x5eeef30b, 0x647c, 0x4341, { 0xb4, 0xe6, 0x8d, 0xa8, 0x8c, 0xe3, 0x26, 0xcd } };

static const GUID guid_restoreBookmark = { 0xc23afd1a, 0xf7bd, 0x4b8f, { 0xa0, 0xff, 0xd, 0xf2, 0x27, 0x1e, 0xe7, 0x48 } };
static const GUID guid_restoreBookmarkActivePlaylistLastPlayed = { 0xafc94d45, 0x65cf, 0x4b41, { 0x97, 0x7e, 0x52, 0xdf, 0xb2, 0xde, 0x38, 0x25 } };
static const GUID guid_clearBookmarks = { 0x2e65ef5a, 0x8620, 0x4cee, { 0xaf, 0x8f, 0xac, 0xd0, 0x6, 0x97, 0x7b, 0xa9 } };

static const GUID guid_startRecording = { 0xe93927e9, 0xc0bd, 0x4485, { 0x8c, 0xa6, 0x26, 0xb8, 0x81, 0x9c, 0x89, 0x95 } };
static const GUID guid_stopRecording = { 0x42b81436, 0xf423, 0x4341, { 0xa1, 0xcf, 0x1a, 0xab, 0x99, 0x24, 0x59, 0x58 } };


static mainmenu_group_popup_factory g_mainmenu_group(guid_vbookmark_main_menu_group_id, mainmenu_groups::playback, mainmenu_commands::sort_priority_dontcare, COMPONENT_NAME_HC);

//ref. to bookmark_dialog.cpp
void bbookmarkHook_store();
void bbookmarkHook_store_selected(metadb_handle_list mhl, bool from_playlist);
void bbookmarkHook_restore();
void bbookmarkHook_restoreActivePlaylist(size_t last_played, bool check_file = false);
void bbookmarkHook_clear();

bool bbookmarkHook_canStore();
bool bbookmarkHook_canStoreSelected();
bool bbookmarkHook_canRestore();
bool bbookmarkHook_canRestoreActivePlaylist(size_t& last_played, bool check_file = false);

bool bbookmarkHook_canClear();

class mainmenu_commands_basic_bookmark : public mainmenu_commands {

public:
	size_t last_played = SIZE_MAX;

	enum {
		cmd_store_selected = 0,
		cmd_store,
		cmd_restore,
		cmd_restoreActivePlaylistLastPlayed,
		cmd_clearBookmarks,

		cmd_startRecording,
		cmd_stopRecording,

		cmd_total
	};

	t_uint32 get_command_count() {
		return cmd_total;
	}

	GUID get_parent() {
		return guid_vbookmark_main_menu_group_id;
	}

	GUID get_command(t_uint32 p_index) {

		switch (p_index) {
		case cmd_store_selected: return guid_storeBookmarkSelected;
		case cmd_store: return guid_storeBookmark;
		case cmd_restore: return guid_restoreBookmark;
		case cmd_restoreActivePlaylistLastPlayed: return guid_restoreBookmarkActivePlaylistLastPlayed;
		case cmd_clearBookmarks: return guid_clearBookmarks;

		case cmd_startRecording: return guid_startRecording;
		case cmd_stopRecording: return guid_stopRecording;

		default: uBugCheck(); // should never happen unless somebody called us with invalid parameters - bail
		}
	}

	void get_name(t_uint32 p_index, pfc::string_base & p_out) {
		switch (p_index) {
		case cmd_store_selected: p_out = "Add Bookmark single selection"; break;
		case cmd_store: p_out = "Add Now Playing Bookmark"; break;
		case cmd_restore: p_out = "Restore Bookmark"; break;
		case cmd_restoreActivePlaylistLastPlayed: p_out = "Restore last bookmark from the active playlist"; break;
		case cmd_clearBookmarks: p_out = "Clear Bookmarks"; break;

		case cmd_startRecording: p_out = "Start Recording"; break;
		case cmd_stopRecording: p_out = "Stop Recording"; break;

		default: uBugCheck(); // should never happen unless somebody called us with invalid parameters - bail
		}
	}

	bool get_description(t_uint32 p_index, pfc::string_base & p_out) {
		switch (p_index) {
		case cmd_store_selected: p_out = "Stores a single selection bookmark"; return true;
		case cmd_store: p_out = "Stores the playback position to a bookmark"; return true;
		case cmd_restore: p_out = "Restores the playback position from the bookmark selected by in the first element to be instantiated."; return true;
		case cmd_restoreActivePlaylistLastPlayed: p_out = "Restores the last bookmark from the active playlist."; return true;
		case cmd_clearBookmarks: p_out = "Removes all bookmarks"; return true;

		case cmd_startRecording: p_out = "Start python script to record sound"; return true;
		case cmd_stopRecording: p_out = "Stop python script to recourd sound"; return true;

		default: uBugCheck(); // should never happen unless somebody called us with invalid parameters - bail
		}
	}

	virtual bool get_display(t_uint32 p_index, pfc::string_base& p_text, t_uint32& p_flags) override {
		p_flags = 0;
		switch (p_index) {
		case cmd_store_selected:
			p_flags = !bbookmarkHook_canStoreSelected();
			break;
		case cmd_store:
			p_flags = !bbookmarkHook_canStore();
			break;
		case cmd_restore:
			p_flags = !bbookmarkHook_canRestore();
			break;
		case cmd_restoreActivePlaylistLastPlayed:
			p_flags = !bbookmarkHook_canRestoreActivePlaylist(last_played);
			break;
		case cmd_clearBookmarks:
			p_flags = !bbookmarkHook_canClear();
			break;

		case cmd_startRecording:
			p_flags = flag_defaulthidden | (glb::g_bmAuto.IsRecording(true, cfg_dst_rec_path.get()) ? flag_disabled : 0);
			break;
		case cmd_stopRecording:
			p_flags = flag_defaulthidden | (glb::g_bmAuto.IsRecording(true, cfg_dst_rec_path.get()) ? 0 : flag_disabled);
			break;

		}
		get_name(p_index, p_text);
		return true;
	}

	void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) {

		switch (p_index) {
		case cmd_store_selected: {

			metadb_handle_list mhl;
			ui_selection_manager::get()->get_selection(mhl);

			auto guid_type = ui_selection_manager::get()->get_selection_type();
			bool bfrom_playlist = pfc::guid_equal(guid_type, contextmenu_item::caller_active_playlist_selection)
				|| pfc::guid_equal(guid_type, contextmenu_item::caller_active_playlist);

			if (bbookmarkHook_canStoreSelected())
				bbookmarkHook_store_selected(mhl, bfrom_playlist);
			break;
		}
		case cmd_store:
			if (bbookmarkHook_canStore())
				bbookmarkHook_store();
			break;
		case cmd_restore:
			if (bbookmarkHook_canRestore())
				bbookmarkHook_restore();
			break;
		case cmd_restoreActivePlaylistLastPlayed: {
			size_t last_played = SIZE_MAX;
			if (bbookmarkHook_canRestoreActivePlaylist(last_played, true)) {
				bbookmarkHook_restoreActivePlaylist(last_played, true);
				last_played = SIZE_MAX;
			}
			break;
		}
		case cmd_clearBookmarks:
			if (bbookmarkHook_canClear())
				bbookmarkHook_clear();
			break;

		case cmd_startRecording:
			if (cfg_dst_rec_path.get().get_length())
				glb::g_bmAuto.StartRecording(glb::g_guiLists, true, cfg_dst_rec_path.get());
			break;
		case cmd_stopRecording:
			if (cfg_dst_rec_path.get().get_length())
				glb::g_bmAuto.StartRecording(glb::g_guiLists, false, cfg_dst_rec_path.get());
			break;

		default: uBugCheck();
		}
	}
};

static mainmenu_commands_factory_t<mainmenu_commands_basic_bookmark> g_mainmenu_commands_basic_bookmark_factory;
