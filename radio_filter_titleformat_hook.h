#pragma once
#include "stdafx.h"

class radio_filter_titleformat_hook : public titleformat_hook {
public:
	radio_filter_titleformat_hook(std::vector<pfc::string8> vfilters = {})  { 

		setData(vfilters);
	}

	virtual bool process_field(titleformat_text_out *p_out,
		const char *p_name,
		t_size p_name_length,
		bool &p_found_flag);

	virtual bool process_function(titleformat_text_out *p_out,
		const char *p_name,
		t_size p_name_length,
		titleformat_hook_function_params *p_params,
		bool &p_found_flag);

	void setData(const std::vector<pfc::string8> vfilters);

private:

	std::vector<pfc::string8> m_vfilters;

};
