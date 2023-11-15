#include "stdafx.h"
#include "bookmark_core.h"
#include "bookmark_persistence.h"
#include "bookmark_preferences.h"
#include "bookmark_worker.h"

double g_pendingSeek;

using namespace glb;

bookmark_worker::bookmark_worker()
{
	//..
}


bookmark_worker::~bookmark_worker()
{
	//..
}

void bookmark_worker::store(std::vector<bookmark_t>& masterList) {
	pfc::string_formatter songDesc;
	bookmark_t newMark = bookmark_t();

	metadb_handle_ptr dbHandle_item;
	auto playback_control_ptr = playback_control::get();
	if (!playback_control_ptr->get_now_playing(dbHandle_item)) {
		//We can not obtain the currently playing item - fizzle out
		FB2K_console_print_e("Get_now_playing failed, can only store time.");
		songDesc << "Could not find playing song info.";

		newMark.set_time(playback_control_ptr->playback_get_position());
		newMark.desc = songDesc;
		newMark.playlist = "";
		newMark.guid_playlist = pfc::guid_null;
		newMark.path = "";
		newMark.subsong = 0;
		gimme_time(newMark);
	}
	else {
		titleformat_object::ptr desc_format;
		static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(desc_format, cfg_desc_format.get_value().c_str());

		if (!dbHandle_item->format_title(NULL, songDesc, desc_format, NULL)) {
			songDesc << "Could not generate Description.";
		}

		//TODO: graceful failure?!
		pfc::string_fixed_t<80> playing_pl_name = "Could not read playlist name.";
		size_t index_playlist;
		GUID guid_playlist = pfc::guid_null;
		size_t index_item;
		auto playlist_manager_ptr = playlist_manager_v5::get();
		if (playlist_manager_ptr->get_playing_item_location(&index_playlist, &index_item)) {
			playlist_manager_ptr->playlist_get_name(index_playlist, playing_pl_name);
			guid_playlist = playlist_manager_v5::get()->playlist_get_guid(index_playlist);
		}

		pfc::string8 songPath = dbHandle_item->get_path();

		newMark.set_time(playback_control_ptr->playback_get_position());
		newMark.desc = songDesc;
		newMark.playlist = playing_pl_name.c_str();
		newMark.guid_playlist = guid_playlist;
		newMark.path = songPath;
		newMark.subsong = dbHandle_item->get_subsong_index();
		gimme_time(newMark);

		if (newMark.isRadio()) {
			titleformat_object::ptr p_script;
			pfc::string8 titleformat = cfg_desc_format.get_value();
			static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(p_script, titleformat);

			pfc::string_formatter songDesc;
			if (playback_control::get()->playback_format_title(NULL, songDesc, p_script, NULL, playback_control::display_level_all)) {
				newMark.desc = songDesc.c_str();
			}
		}
	}

	masterList.emplace_back(newMark);
}

void bookmark_worker::restore(std::vector<bookmark_t>& masterList, size_t index) {
	if (masterList.empty()) {	//Nothing to restore
		FB2K_console_print_v("Restore Bookmark failed...no bookmarks.");
		return;
	}

	if (index >= 0 && index < masterList.size()) {	//load using the index
		auto rec = masterList[index];

		abort_callback_impl p_abort;
		try {
			if (!filesystem_v3::g_exists(rec.path.c_str(), p_abort)) {
				FB2K_console_print_e("Restore Bookmark failed...object not found.");
				return;
			}
		}
		catch (exception_aborted) {
			return;
		}

		g_bmAuto.updateRestoredDummy(rec);

		if (!(bool)rec.path.get_length()) {
			FB2K_console_print_v("Restore Bookmark failed...no track in bookmark.");
			return;
		}

		//restore track:
		auto metadb_ptr = metadb::get();
		auto playlist_manager_ptr = playlist_manager::get(); //Get index of stored playlist
		auto playback_control_ptr = playback_control::get();

		metadb_handle_ptr track_bm = metadb_ptr->handle_create(rec.path.c_str(), rec.subsong);	//Identify track to restore

		size_t index_pl = ~0;
		size_t index_item = ~0;

		{
			std::lock_guard<std::mutex> ul(g_mtx_restoring);
			if (g_restoring == true) {
			
				if (g_pendingSeek == 0.0) { //If a time change was not queued up, the track is either already correct or could not be determined
					if (!core_api::assert_main_thread()) {
						FB2K_console_print_v("(Not in main thread)");
					}

					FB2K_console_print_v("Restoring time:", rec.get_time());

					playback_control_ptr->playback_seek(rec.get_time());
				}

				//unpause
				playback_control_ptr->pause(false);
			}
		}
	}
	else {	//Index invalid, fall back to the last entry
		restore(masterList, masterList.size() - 1);
		return;
	}
}


// worker play callback

class bm_worker_play_callback : public play_callback_static {
public:
	/* play_callback methods go here */
	void on_playback_starting(play_control::t_track_command p_command, bool p_paused) override {}
	void on_playback_stop(play_control::t_stop_reason p_reason) override {}
	void on_playback_pause(bool p_state) override {}
	void on_playback_edited(metadb_handle_ptr p_track) override {}
	void on_playback_dynamic_info(const file_info & p_info) override {}
	void on_playback_dynamic_info_track(const file_info & p_info) override {}
	void on_volume_change(float p_new_val) override {}
	void on_playback_seek(double p_time) override {}
	void on_playback_time(double p_time) override {}

	//To apply the seek to the right track, we need to wait for it to start playing;
	//and to prevent race conditions we need to get the actual seeking out of the on_playback
	//(by scheduling it in the main thread)

	void on_playback_new_track(metadb_handle_ptr p_track) override {

		if (g_pendingSeek != 0.0) {

			//Lambda can't capture the global variable directly

			fb2k::inMainThread([d = g_pendingSeek] {
				if (playback_control::get()->playback_can_seek()) {
					playback_control::get()->playback_seek(d);
				}
			});

			g_pendingSeek = 0.0;

		}
	}

	// select worker play callbacks

	virtual unsigned get_flags() {

		return flag_on_playback_new_track;
	}
};

static service_factory_single_t<bm_worker_play_callback> g_worker_play_callback_static_factory;
