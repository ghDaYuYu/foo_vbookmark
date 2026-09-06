#include "bookmark_core.h"
#include "bookmark_worker.h"
#include "bookmark_list_control.h"

using namespace glb;

namespace {

	class initquit_bbookmark : public initquit {

		virtual void on_init() {

			g_store.Initialize();

			if (is_cfg_Play_OnInit() &&
					!playback_control::get()->is_playing() &&
					!playlist_manager::get()->queue_get_count()) {

				FB2K_console_print_v("Restoring last bookmark on startup.");
				size_t cmaster = g_store.GetMasterList().size();

				if (cmaster) {

					bookmark_worker bmWorker;
					bmWorker.restore(cmaster - 1);

					if (is_cfg_Rq_OnInit()) {

						bookmark_t bm = g_store.GetItem(cmaster - 1);

						metadb_handle_list mhl;
						metadb_handle_ptr mhp;

						bool res = playback_control::get()->get_now_playing(mhp);
						mhl.add_item(mhp);

						ThreadUtils::cmdThread cmd;
						cmd.add([mhl]() {

							Sleep(atoi(cfg_rq_wait.get()) * 1000);

							fb2k::inMainThread([mhl]() {

								menu_helpers::name_to_guid_table menu_table;

								GUID guid_fbn;
								bool res = mainmenu_commands_v3::g_find_by_name("Restore last session queue", guid_fbn);
								if (res) {
									FB2K_console_print_v("Restoring queue on startup.");
									mainmenu_commands_v3::g_execute(guid_fbn);
								}
								}); //main thread
							}); //thread pool
					}
				}
			}
			if (g_primaryGuiList) {
				g_primaryGuiList->ReloadData();
				g_primaryGuiList->RestoreLastFocus();
			}
		}

		virtual void on_quit() {

			if (is_cfg_Bookmarking() && cfg_autosave_on_quit.get()) {

				if (g_bmAuto.checkDummy()) {

					g_store.AddItem(g_bmAuto.getDummy());
				}
			}

			g_store.Write(false);
		}
	};

	// S T A T I C   I N I T / Q U I T

	static initquit_factory_t< initquit_bbookmark > g_bookmark_initquit;
}