#include "stdafx.h"
#include "utils.h"
#include "radio_filter_titleformat_hook.h"

void radio_filter_titleformat_hook::setData(std::vector<pfc::string8> vfilters) {
	m_vfilters = vfilters;
}

bool radio_filter_titleformat_hook::process_field(titleformat_text_out *p_out,
		const char *p_name,
		t_size p_name_length,
		bool &p_found_flag) {
		p_found_flag = false;
		return false;
}

bool radio_filter_titleformat_hook::process_function(titleformat_text_out *p_out,
	const char * p_name,
	t_size p_name_length,
	titleformat_hook_function_params * p_params,
	bool & p_found_flag) {

	if (!stricmp_utf8_ex(p_name, p_name_length, "in_radio_filters", pfc_infinite)) {
		p_found_flag = true;
		if (p_params->get_param_count() == 1) {
			const char* name;
			t_size name_length;
			p_params->get_param(0, name, name_length);

			//todo: rev. checkfilter() in Prefs and upgradeDummy
			filters::radio_nfo_type rnt;
			filters::get_radio_nfo(name, rnt);

			pfc::string8 sigstr = name;
			if (filters::check_radio_signature(radio_lensig != SIZE_MAX) && primary_ok != SIZE_MAX && primary_ok >= 2) {
				sigstr = sigstr.subString(0, rnt.primary_sig.second);
			}
			//

			auto it = std::find_if(m_vfilters.begin(), m_vfilters.end(), [sigstr](const pfc::string8 s) {
				return s.equals(sigstr);});

			if (it == m_vfilters.end()) {
				p_found_flag = true;
				p_out->write(titleformat_inputtypes::unknown, "", pfc_infinite);
				return false;
			}
			else {
				p_found_flag = true;
				p_out->write(titleformat_inputtypes::unknown, it->get_ptr(), pfc_infinite);
				return true;
			}
		}
		else {
			p_out->write(titleformat_inputtypes::unknown, "[INVALID PARAMS]", pfc_infinite);
			p_found_flag = true;
			return true;
		}
		return false;
	}
	else if (!stricmp_utf8_ex(p_name, p_name_length, "cont_radio_filters", pfc_infinite)) {
		p_found_flag = true;
		if (p_params->get_param_count() == 1) {
			const char* name;
			t_size name_length;
			p_params->get_param(0, name, name_length);

			//todo: rev. checkfilter() in Prefs and upgradeDummy
			filters::radio_nfo_type rnt;
			filters::get_radio_nfo(name, rnt);
			size_t primary_ok = rnt.primary_sig.first;
			size_t radio_lensig = rnt.primary_sig.second;

			pfc::string8 sigstr = name;
			if (filters::check_radio_signature(radio_lensig != SIZE_MAX) && primary_ok != SIZE_MAX && primary_ok >= 2) {
				sigstr = sigstr.subString(0, rnt.primary_sig.second);
			}
			//

			auto it = std::find_if(m_vfilters.begin(), m_vfilters.end(), [sigstr](const pfc::string8 s) {
				return strstr(sigstr, s.c_str()) != nullptr; });

			if (strlen(name) == 0 || it == m_vfilters.end()) {
				p_found_flag = true;
				p_out->write(titleformat_inputtypes::unknown, "", pfc_infinite);
				return false;
			}
			else {
				p_found_flag = true;
				p_out->write(titleformat_inputtypes::unknown, it->get_ptr(), pfc_infinite);
				return true;
			}
		}
		else {
			p_out->write(titleformat_inputtypes::unknown, "[INVALID PARAMS]", pfc_infinite);
			p_found_flag = true;
			return true;
		}
		return false;
	}
	else
		return false;
}