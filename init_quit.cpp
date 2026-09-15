#include "bookmark_core.h"
#include "bookmark_worker.h"
#include "bookmark_list_control.h"
#include "bookmark_store.h"

using namespace glb;

namespace {

	class initquit_bbookmark : public initquit {

		virtual void on_init() {

			//todo: remove callbacks
			std::function add_bookmark_callback([](/) {
				//todo: NoRefreshScope
				bookmark_store::set_no_refresh(false);

				fb2k::inMainThread([]() {

					FB2K_console_print_v("Restoring last bookmark on startup.");
					size_t cmaster = g_store.GetMasterList().size();

					if (cmaster) {

						for (std::list<dlg::CListControlBookmark*>::iterator it = g_guiLists.begin(); it != g_guiLists.end(); ++it) {
							(*it)->ShowWindow(SW_SHOW);
							(*it)->ReloadItems(bit_array_true());
							(*it)->OnItemsInserted(cmaster - 1, cmaster, false);
							(*it)->Invalidate(true);
						}
						if (g_primaryGuiList) {
							g_primaryGuiList->RestoreLastFocus();
						}

						if (is_cfg_Play_OnInit() &&
							!playback_control::get()->is_playing() &&
							!playlist_manager::get()->queue_get_count()) {

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
				});
				});

			ThreadUtils::cmdThread cmd;
			cmd.add([add_bookmark_callback] {

				bool ordered = false;
				if (GetPrimaryGuiList()) {
					ordered = g_primaryGuiList->GetSortOrder();
				}

				bookmark_t bm_loading;
				bm_loading.set_desc("loading");
				bm_loading.set_current_date();
				g_store.AddItem(bm_loading, std::function<void()>([]() {
					if (GetPrimaryGuiList()) {
						fb2k::inMainThread([]() {

							for (std::list<dlg::CListControlBookmark*>::iterator it = g_guiLists.begin(); it != g_guiLists.end(); ++it) {
								(*it)->ShowWindow(SW_SHOW);
								(*it)->ReloadData();
								(*it)->ReloadItems(bit_array_true());
								(*it)->ShowScrollBar(SB_VERT, true);
								(*it)->Invalidate(true);
							}

							ThreadUtils::cmdThread cmd_set; cmd_set.add([]() {
								//todo: NoRefreshScope
								Sleep(100);
								});
							});
					}
					}));

				bool done = g_store.Initialize(ordered, add_bookmark_callback);
			});
		}

		virtual void on_quit() {

			if (is_cfg_Bookmarking() && cfg_autosave_on_quit.get()) {

				if (g_bmAuto.checkDummy()) {

					g_store.AddItem(g_bmAuto.getDummy(),
						//todo: remove callbacks
						std::function<void()>([]() { g_store.Write(false); }));
				}
				//
				return;
				//
			}
			g_store.Write(false);
		}
	};

	// S T A T I C   I N I T / Q U I T

	static initquit_factory_t< initquit_bbookmark > g_bookmark_initquit;
}