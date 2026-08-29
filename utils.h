#pragma once

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
}
