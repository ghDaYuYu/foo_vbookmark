#include "bookmark_core.h"
#include "bookmark_automatic.h"
#include "bookmark_play_callback.h"

using namespace glb;

namespace {

	// stop

	void bm_play_callback::on_playback_stop(play_control::t_stop_reason p_reason) {

		if (!cfg_monitor.get()) {
			return;
		}

		if (p_reason == play_control::stop_reason_shutting_down) {
			if (cfg_autosave_on_quit.get()) {
				g_bmAuto.updateDummy();
			}
		}
		else {
			if (g_bmAuto.getDummy().isRadio()) {
				//.. reset in time event
			}
			else {
				g_bmAuto.ResetRestoredDummy();
			}
			g_bmAuto.resetDummyAll();
			g_bmAuto.updateDummy();
		}
	}

	// new track

	void bm_play_callback::on_playback_new_track(metadb_handle_ptr p_track) {

		if (!cfg_monitor.get()) {
			return;
		}

		if (g_bmAuto.getDyna()) {
			FB2K_console_print_v("New dyna track event...");
		}
		else {
			FB2K_console_print_v("New track event...");
		}

		if (g_wnd_bookmark_pref) {
			SendMessage(g_wnd_bookmark_pref, UMSG_NEW_TRACK, NULL, NULL);
		}

		if (g_bmAuto.getDyna()) {
			g_bmAuto.resetDummyKeepDyna();
		}
		else {
			g_bmAuto.resetDummyAll();
		}

		g_bmAuto.updateDummy();
		//todo: remove m_updating from updateDummy()
		g_bmAuto.Reset_Updating();

		g_bmAuto.setDyna(false);

		auto bm = g_bmAuto.getDummy();

		g_bmAuto.ResetRestoredDummyTime();

		if (g_wnd_bookmark_pref) {
			SendMessage(g_wnd_bookmark_pref, UMSG_NEW_TRACK, NULL, NULL);
		}

		return;

	}

	// seek

	void bm_play_callback::on_playback_seek(double p_time) {

		if (!cfg_monitor.get()) {
			return;
		}

		if (is_cfg_Bookmarking() && is_cfg_LapseEnabled()) {
			//
			g_bmAuto.cancelUpdating();
			//
		}
	}

	// time

	void bm_play_callback::on_playback_time(double p_time) {

		if (!cfg_monitor.get()) {
			return;
		}

		g_bmAuto.updateDummyTime();
	}

	void bm_play_callback::on_playback_dynamic_info_track(const file_info& p_info) {

		if (!cfg_monitor.get()) {
			return;
		}

		FB2K_console_print_v("Dynamic info arrived...");

		if (!g_bmAuto.checkDummyIsRadio()) {
			return;
		}

		{
			auto playback_control_ptr = playback_control::get();

			metadb_handle_ptr track_current;
			//Identify current track
			bool bnowPlaying = playback_control_ptr->get_now_playing(track_current);

			if (bnowPlaying) {

				g_bmAuto.setDyna(true);

				//
				on_playback_new_track(track_current);
				//
			}
		}
	}
}

// S T A T I C   P L A Y - C A L L B A C K

static service_factory_single_t<bm_play_callback> g_play_callback_static_factory;
