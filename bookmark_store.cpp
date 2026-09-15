#include "stdafx.h"
#include "bookmark_store.h"

using bm_rev_it_type = std::vector<bookmark_t>::reverse_iterator;
using keep_map_rev_it_type = std::map <std::string, bm_rev_it_type>::reverse_iterator;

std::vector<bookmark_t> bookmark_store::Discard_Bookmarks(std::vector<bookmark_t> master_list) {

	size_t keep_n_latest = is_cfg_cu_keep_tail() ? atoi(cfg_cu_keep_tail_count.get_value()) : 0;
	pfc::string8 keep_comment_prefix= cfg_cu_keep_com_prefix;
	bool keep_non_radio_seek = is_cfg_cu_keep_seek();
	bool keep_backup = is_cfg_cu_keep_backup();

	if (keep_backup) {
		pfc::string8 n8_path = core_api::pathInProfile("configuration");
		extract_native_path(n8_path, n8_path);
		std::filesystem::path os_dat = std::filesystem::u8path(n8_path.c_str());
		std::filesystem::path os_bak = std::filesystem::u8path(n8_path.c_str());

		pfc::string8 file_date;
		currentFmtDate(file_date, "_%y-%m-%d_%Hh_%Mm_%Ss");
		pfc::string8 filename = core_api::get_my_file_name();
		pfc::string8 filename_bak = core_api::get_my_file_name();
		filename << ".dll.dat";
		filename_bak << file_date << ".dll.dat";
		os_dat.append(filename.c_str());
		os_bak.append(filename_bak.c_str());

		std::error_code ec;
		if (std::filesystem::exists(os_dat)) {
			std::filesystem::copy_file(os_dat, os_bak, std::filesystem::copy_options::overwrite_existing, ec);
		}
	}

	size_t keep_n_latest_fixed = (master_list.size() >= keep_n_latest ? keep_n_latest : master_list.size());

	std::map<std::string, bm_rev_it_type> keep_bms;
	bm_rev_it_type it = master_list.rbegin() + keep_n_latest_fixed;

	while (it != master_list.rend()) {

		it = std::find_if(it, master_list.rend(), [](const auto bmit) { return (bool)bmit.guid_playlist.Data1; });

		if (it == master_list.rend()) {
			//
			continue;
			//
		}

		keep_map_rev_it_type keep_rev_it = std::find_if(keep_bms.rbegin(), keep_bms.rend(),
			[it](const std::pair<std::string, bm_rev_it_type>& kbm) {

				bool bres = (bool)it->guid_playlist.Data1;
				return bres && pfc::guid_equal(pfc::GUID_from_text(kbm.first.c_str()), it->guid_playlist);

			});

		if (keep_rev_it  == keep_bms.rend()) {
			keep_bms.emplace(std::pair{ pfc::print_guid(it->guid_playlist), it });

		};

		it = std::find_if(it, master_list.rend(), [it](bookmark_t const bm) { return !pfc::guid_equal(bm.guid_playlist, it->guid_playlist); });
	}

	bm_rev_it_type& it_delete = master_list.rbegin() + keep_n_latest_fixed;

	std::vector<std::string> v_pl_done;

	while (it_delete != master_list.rend()) {

		size_t curr_ndx = master_list.size() - std::distance(it_delete, master_list.rend());

		bool pre_cond = (keep_comment_prefix.get_length() && it_delete->get_comment().startsWith(keep_comment_prefix)) ||
				(keep_non_radio_seek && !it_delete->isRadio() && it_delete->get_time() != 0.0);

		if ((bool)it_delete->guid_playlist.Data1 && !pre_cond) {

			bool bwasdone = std::find(v_pl_done.begin(), v_pl_done.end(),
				pfc::print_guid(it_delete->guid_playlist).c_str()) != v_pl_done.end();
	
			if (bwasdone) {

				it_delete->subsong = 9999;
				it_delete++;
				continue;
			}
		}

		keep_map_rev_it_type/*auto*/ keep_find_rev_it = std::find_if(keep_bms.rbegin(), keep_bms.rend(),
			[it_delete](const std::pair<std::string, bm_rev_it_type>& kbm) {
				return it_delete == kbm.second; });

		if (keep_find_rev_it != keep_bms.rend()) {

			it_delete++;

			v_pl_done.push_back(pfc::print_guid(keep_find_rev_it->second->guid_playlist).c_str());

			//
			continue;
			//

		}

		it_delete++;
	}

	int to_remove = (std::max)((int)(master_list.size() - keep_n_latest - keep_bms.size()), 0);
	size_t c_removed = 0;

	auto new_end = std::remove_if(master_list.begin(), master_list.end() - keep_n_latest_fixed,
			[to_remove, &c_removed, keep_comment_prefix, keep_non_radio_seek](bookmark_t bm) {

		bool pre_cond = (keep_comment_prefix.get_length() && bm.get_comment().startsWith(keep_comment_prefix)) ||
				(keep_non_radio_seek && !bm.isRadio() && bm.get_time() != 0.0);
		
		if (!pre_cond && ((c_removed < to_remove && !(bool)bm.guid_playlist.Data1) || bm.subsong == 9999)) {
			c_removed++;
			return true;
		}
		return false;
	});

	std::vector<bookmark_t> back;
	back.insert(back.begin(), master_list.end() - keep_n_latest_fixed, master_list.end());

	master_list.erase(new_end, master_list.end());
	master_list.insert(master_list.end(), back.begin(), back.end());

	return master_list;
}

bookmark_store::bookmark_store() {
	//..
}

bookmark_store::~bookmark_store()
{
	//..
}
