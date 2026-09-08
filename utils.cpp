#include "stdafx.h"
#include <regex>
#include <algorithm>

#include "pugixml.hpp"

#include "utils.h"

namespace filters {

	pfc::string8 ltrim(const pfc::string8& str, const char* ch) {
		std::string dest((const char*)str);
		dest.erase(0, dest.find_first_not_of(ch));
		return pfc::string8(dest.c_str());
	}

	pfc::string8 rtrim(const pfc::string8& str, const char* ch) {
		std::string dest((const char*)str);
		dest.erase(dest.find_last_not_of(ch) + 1);
		return pfc::string8(dest.c_str());
	}

	pfc::string8 trim(const pfc::string8& str, const char* ch) {
		return ltrim(rtrim(str, ch), ch);
	}

	pfc::string8 trim(const pfc::string8& str) {
		return trim(str, whitespace);
	}

	bool is_dyna_double_pipe(const pfc::string radio_info) {
		bool piped = false;
		pfc::chain_list_v2_t<pfc::string8> split_list_dbl_pipe;
		pfc::splitStringBySubstring(split_list_dbl_pipe, radio_info, "||");
		if (split_list_dbl_pipe.get_count() >= kMinRadioFields) {
			piped = true;
		}
		return piped;
	}

	size_t check_radio_signature(size_t radio_lensig) {
		return radio_lensig != SIZE_MAX && radio_lensig >= (kMinRadioFields + kMinRadioFieldsLen) ? radio_lensig : SIZE_MAX;
	}

	//todo

	bool check_reversed(const std::vector<pfc::string8> vec) {
		//rev cap done before this
		size_t cdates = 0;
		std::smatch result;
		std::regex pattern("\\d{4}[-]\\d{2}[-]\\d{2}[t]");
		for (size_t w = kMinRadioFields; w < vec.size(); w++) {
			const std::string buf(vec[w].toLower());
			if (std::regex_search(buf, result, pattern)) {
				cdates++;
			}
		}

		// cdates > 1 and year non numeric
		//"EP~2016~~201~2026-08-26T19:54:40~2026-08-26T19:54:40~Radio - ARTIST~SONG~ALBUM"
		return (cdates > 1 && vec[3].get_length() != 0 && !pfc::string_is_numeric(vec[3]));
	}

	void transform_if_first_of_word(char& c)
	{
		if ((*(&c - sizeof(char))) == ' ' || (*(&c - sizeof(char))) == '.' || (*(&c - sizeof(char))) == '-')
			c = toupper(c);
	}

	std::pair<size_t, size_t> get_radio_info_sigfields(const pfc::string8 radio_info, std::vector<pfc::string8>& vout) {

		std::pair<size_t, size_t> primaries_sig_len = std::pair(SIZE_MAX, SIZE_MAX);

		if (!trim(radio_info).get_length()) {
			vout.clear();
			//
			return primaries_sig_len;
			//
		}

		//todo

		pfc::chain_list_v2_t<pfc::string8> split_list;
		pfc::chain_list_v2_t<pfc::string8> split_list_dbl_pipe;
		pfc::splitStringByChar(split_list, radio_info, '~');
		pfc::splitStringBySubstring(split_list_dbl_pipe, radio_info, "||");

		bool piped = false;

		if (split_list.get_count() >= kMinRadioFields) {
			if (split_list.get_count() > kMinRadioFields + 1) { //+1 foo_vbookmark
				pfc::chain_list_v2_t<pfc::string8> split_list_bunch;
				pfc::string8 bunch_artist;
				pfc::splitStringByChar(split_list_bunch, split_list.by_index(split_list.get_count() - 1).get()->c_str(), '-');
				if (split_list_bunch.get_count() < 5) { //or all uppercase
					//artist song album
					const pfc::string8 song = split_list.by_index(1).get()->c_str();
					split_list.by_index(1).get()->set_string(split_list.by_index(0).get()->c_str());
					split_list.by_index(0).get()->set_string(song);
				}
				
			}
			else if (split_list.get_count() <= kMinRadioFields + 1) {
				//artist album song
				//const pfc::string8 artist = split_list.by_index(0).get()->c_str();
				const pfc::string8 album = split_list.by_index(2).get()->c_str();
				const pfc::string8 song = split_list.by_index(1).get()->c_str();
				split_list.by_index(2).get()->set_string(album); // album
				split_list.by_index(1).get()->set_string(split_list.by_index(0).get()->c_str()); // artist
				split_list.by_index(0).get()->set_string(song); // song
			}
			else {
				//..
			}
		}
		else if (split_list_dbl_pipe.get_count() >= kMinRadioFields) {
			piped = true;
			split_list.remove_all(); split_list.move_from(split_list_dbl_pipe);
			pfc::string8 bunch_artist = split_list.by_index(kMinRadioFields - 1).get()->c_str();
			pfc::chain_list_v2_t<pfc::string8> split_list_bunch;
			pfc::splitStringByChar(split_list_bunch, bunch_artist, '-');
			if (split_list_bunch.get_count() == 6) {
				bunch_artist = trim(split_list_bunch.by_index(split_list_bunch.get_count() - 1).get()->c_str());
				split_list.by_index(kMinRadioFields - 1).get()->set_string(split_list.by_index(1).get()->c_str());
				split_list.by_index(1).get()->set_string(bunch_artist);
				split_list.add_item(split_list.by_index(2).get()->c_str());
				split_list.by_index(2).get()->set_string("");
			}
			else {
				bunch_artist = split_list.by_index(1).get()->c_str();
				pfc::splitStringByChar(split_list_bunch, bunch_artist, '-');
				if (split_list_bunch.get_count() == 2) {
					bunch_artist = trim(split_list_bunch.by_index(split_list_bunch.get_count() - 1).get()->c_str());
					split_list.by_index(kMinRadioFields - 1).get()->set_string(split_list.by_index(1).get()->c_str());
					split_list.by_index(1).get()->set_string(trim(split_list_bunch.by_index(0).get()->c_str()));
					split_list.by_index(0).get()->set_string(trim(split_list_bunch.by_index(1).get()->c_str()));
					split_list.add_item(split_list.by_index(2).get()->c_str());
					split_list.by_index(2).get()->set_string("");
				}
			}
		}
		else {
			vout.clear();
			//
			return primaries_sig_len;
			//
		}

		vout.clear();
		vout.resize(split_list.get_count());

		//basic sig (fld~fld2~fld3~fld4~) length
		size_t sig_len = 0;
		size_t primary_ok = 0;

		bool bcap = vout.size() > kMinRadioFields;

		for (size_t w = 0; w < vout.size(); w++) {

			vout[w] = trim(split_list.by_index(w).get()->c_str());

			if (bcap && vout[w].get_length() && vout[w].upperCase().equals(vout[w])) {
				vout[w] = vout[w].toLower();
				vout[w].set_char(0, pfc::charUpper(vout[w].firstChar()));
				std::string str = vout[w];
				std::for_each(str.begin() + 1, str.end(), transform_if_first_of_word);
				vout[w] = str.c_str();
			}

			size_t wlen = vout[w].get_length();

			if (w == 0) {
				if (wlen) {
					primary_ok++;
				}
				else {
					primary_ok = SIZE_MAX;
				}
			}
			else {
				if (primary_ok != SIZE_MAX && w <= kMinRadioFields && wlen) {
					primary_ok++;
				}
			}
			if (w <= kMinRadioFields) {
				sig_len += wlen;
				++sig_len;
			}
		}

		//todo

		if (!piped && vout.size() > kMinRadioFields && check_reversed(vout)) {
			//reverse
			std::reverse(vout.begin(), vout.end());
			primary_ok = 0;
			sig_len = 0;

			for (size_t w = 0; w < vout.size(); w++) {
				size_t wlen = vout[w].get_length();
				if (w == 0) {
					if (wlen) {
						primary_ok++;
					}
					else {
						primary_ok = SIZE_MAX;
					}
				}
				else {
					if (primary_ok != SIZE_MAX && w <= kMinRadioFields && wlen) {
						primary_ok++;
					}
				}
				if (w <= kMinRadioFields) {
					sig_len += wlen;
					++sig_len;
				}
			}
		}

		primaries_sig_len = std::pair(primary_ok, sig_len);

		return primaries_sig_len;
	}

	size_t get_filters(const pfc::string8 csv_filter, std::vector<pfc::string8>& vout) {

		if (!trim(csv_filter).get_length()) {
			vout.clear();
			return 0;
		}

		size_t ires = SIZE_MAX;
		pfc::chain_list_v2_t<pfc::string8> split_list;
		pfc::splitStringByChar(split_list, csv_filter, ',');
		vout.clear();

		for (size_t i = 0; i < split_list.get_count(); i++) {
			pfc::string8 buf = trim(split_list.by_index(i).get()->c_str());
			if (buf.length()) {
				vout.emplace_back(buf);
			}
		}
		return ires;
	}

	std::pair<size_t, size_t> filters_in_fields(const std::vector<pfc::string8> vinfo, const std::vector<pfc::string8> vfilter) {

		std::pair<size_t, size_t> pres(SIZE_MAX, SIZE_MAX);
		size_t cinfo = vinfo.size();
		auto it = std::find_if(vinfo.begin(), vinfo.end(), [&pres, vfilter, cinfo](const pfc::string8 s) {
			auto itf = std::find_if(vfilter.begin(), vfilter.end(), [s, cinfo](const pfc::string8 f) {
				bool bres = cinfo > kMinRadioFields ? s.equals(f) : s.startsWith(f);
				return bres; });
			if (itf != vfilter.end()) {
				pres.second = std::distance(vfilter.begin(), itf);
				return true;
			}
			return false;
			});
		if (it != vinfo.end()) {
			pres.first = std::distance(vinfo.begin(), it);
		}
		return pres;
	}

	//
	// in:      radio info txt
	// out:     radio_nfo_type
	// returns: length of basic signature (fld~fld2~fld3~fld4~) or
	//          SIZE_MAX for basic info (fields < kMinRadioFields)
	//

	void get_radio_nfo(const pfc::string8 desc, radio_nfo_type& rnt) {

		pfc::string8 xml_desc;
		std::vector<pfc::string8> vfields;

		if (desc.startsWith("<?xml")) {
	
			pugi::xml_document doc;
			pugi::xml_parse_result result = doc.load_string(desc);
	
			if (result) {

				pfc::string8 title = doc.child("RadioInfo").child("Table").child("DB_DALET_TITLE_NAME").child_value();
				pfc::string8 artist = doc.child("RadioInfo").child("Table").child("DB_DALET_ARTIST_NAME").child_value();
				pfc::string8 album = doc.child("RadioInfo").child("Table").child("DB_ALBUM_NAME").child_value();

				if (title.get_length()) {
					xml_desc = PFC_string_formatter() << title << "~" << artist << "~" << album << "~~" << "vbm";
					xml_desc = filters::autoFixEncoding(xml_desc.c_str()).c_str();
				}
			}
		}

		std::pair<size_t, size_t> primary_sig = get_radio_info_sigfields(xml_desc.get_length() ? xml_desc : desc, rnt.vfields);
		rnt.radio_info = desc;
		rnt.primary_sig = primary_sig;
	}

	size_t parse_radio_info(const radio_nfo_type rnt, titleformat_hook* p_hook, pfc::string8& out, const pfc::string8 tf_format_str) {

		std::vector<pfc::string8> vout = rnt.vfields;
		std::pair<size_t, size_t> primary_info = rnt.primary_sig;

		size_t primary_ok = primary_info.first;
		size_t radio_lensig = primary_info.second;

		if (check_radio_signature(radio_lensig != SIZE_MAX) && primary_ok != SIZE_MAX && primary_ok >= 2) {

			file_info_impl fii;

			for (size_t i = 0; i < kMinRadioFields; i++) {
				if (vout[i].get_length()) {

					// no warranty, could be any iteration (artist, song, album...)
					// but not too relevant for filtering on sig

					switch (i) {
					case 0:
						fii.meta_set("title", vout[i].c_str());
						break;
					case 1:
						fii.meta_set("artist", vout[i].c_str());
						break;
					case 2:
						fii.meta_set("album", vout[i].c_str());
						break;
					case 3:
						fii.meta_set("date", vout[i].c_str());
						break;
					}
				}
			}

			metadb_handle_ptr mhp;
			playback_control_v3::get()->get_now_playing(mhp);

			titleformat_object::ptr tfo;
			static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(tfo, tf_format_str);

			mhp->format_title_from_external_info(fii, p_hook, out, tfo, NULL);
		}
		return check_radio_signature(radio_lensig);
	}
}
