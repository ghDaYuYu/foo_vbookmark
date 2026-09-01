#pragma once
#include <iostream>
#include <string>
#include <regex>

namespace filters {

	constexpr char* whitespace = " \t\r\n";;

	constexpr size_t kMinRadioFields = 4;
	constexpr size_t kMinRadioFieldsLen = 4;

	// Trim whitespace from strings
	inline pfc::string8 trim(const pfc::string8& str, const char* ch);
	inline pfc::string8 trim(const pfc::string8& str);
	inline pfc::string8 ltrim(const pfc::string8& str, const char* ch = whitespace);
	inline pfc::string8 rtrim(const pfc::string8& str, const char* ch = whitespace);

	extern bool is_dyna_double_pipe(const pfc::string radio_info);

	extern size_t get_filters(const pfc::string8 csv_filter, std::vector<pfc::string8>& vout);

	extern std::pair<size_t, size_t> filters_in_fields(const std::vector<pfc::string8> vinfo, const std::vector < pfc::string8> vfilter);

	struct radio_nfo_type {
	pfc::string8 radio_info;
	std::pair<size_t, size_t> primary_sig;
	std::vector<pfc::string8> vfields;
	};

	size_t check_radio_signature(size_t radio_lensig);

	//first: count primary ok, second primary ok signature length
	extern std::pair<size_t, size_t> get_radio_info_sigfields(const pfc::string8 radio_info, std::vector<pfc::string8>& vout);
	extern void get_radio_nfo(const pfc::string8 desc, radio_nfo_type& rnt);
	extern size_t parse_radio_info(const radio_nfo_type rnt, titleformat_hook* p_hook, pfc::string8& out, const pfc::string8 desc_format_str);

	inline void replace_non_utf8(pfc::string8 &out, const char* src, size_t len, char replace = '?') {
		while (len && *src) {
			unsigned c; t_size d;
			d = pfc::utf8_decode_char(src, c, len);
			if (d == 0 /*|| d > len*/) {
				c = '?';
				d = 1;
			}
			out.add_char(c);
			src += d;
			len -= d;
		}
	}

	inline extern std::string autoFixEncoding(const std::string& text) {

		if (text.empty()) {
			//
			return "";
			//
		}

		bool valid_ut8 = pfc::string8(text.c_str()).is_valid_utf8();

		if (valid_ut8) {
			//
			return text;
			//
		}

		std::string result = text;

		//
		// French
		//

		// \b�a\b -> �a
		//result = std::regex_replace(result, std::regex(R"(\b�a\b)"), "�a");
		result = std::regex_replace(result, std::regex(R"(\b�a\b)"), "�a");
		// \bEa\b -> �a
		//result = std::regex_replace(result, std::regex(R"(\bEa\b)"), "�a");
		result = std::regex_replace(result, std::regex(R"(\b�a\b)"), "Ea");
		// \s\?\s ->  � 
		//result = std::regex_replace(result, std::regex(R"(\s\?\s)"), " � ");
		result = std::regex_replace(result, std::regex(R"(\s\�\s)"), " ? ");

		// ([rR])\?ve -> $1�ve (case insensitive)
		//result = std::regex_replace(result, std::regex(R"(([rR])\?ve)", std::regex_constants::icase), "$1�ve");
		result = std::regex_replace(result, std::regex(R"(([rR])\�ve)", std::regex_constants::icase), "$1?ve");
		// (^|\s)\?([a-zA-Z]) -> $1�$2
		//result = std::regex_replace(result, std::regex(R"((^|\s)\?([a-zA-Z]))"), "$1�$2");
		result = std::regex_replace(result, std::regex(R"((^|\s)\�([a-zA-Z]))"), "$1?$2");
		// ([a-zA-Z])\?([a-zA-Z]) -> $1�$2
		//result = std::regex_replace(result, std::regex(R"(([a-zA-Z])\?([a-zA-Z]))"), "$1�$2");
		result = std::regex_replace(result, std::regex(R"(([a-zA-Z])\�([a-zA-Z]))"), "$1?$2");

		// Literal replacements
		// Note: C++ regex handles UTF-8 characters as multi-byte sequences.
		result = std::regex_replace(result, std::regex("�"),"è");
		result = std::regex_replace(result, std::regex("�"),"é");
		result = std::regex_replace(result, std::regex("�"),"� ");
		result = std::regex_replace(result, std::regex("�"),"ç");
		result = std::regex_replace(result, std::regex("�"),"ù");
		result = std::regex_replace(result, std::regex("�"),"ê");
		result = std::regex_replace(result, std::regex("�"),"ë");
		result = std::regex_replace(result, std::regex("�"),"î");
		result = std::regex_replace(result, std::regex("�"),"ï");
		result = std::regex_replace(result, std::regex("�"),"ô");

		//
		// 
		//

		// replace leftovers with '?'

		pfc::string8 temp_result = result.c_str();
		if (!temp_result.is_valid_utf8()) {

			replace_non_utf8(temp_result, result.data(), result.size());
			result = temp_result.c_str();

		}
		return result;
	}
}
