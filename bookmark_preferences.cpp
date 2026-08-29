#include "stdafx.h"
#include "resource.h"

#include <vector>
#include <list>
#include <sstream>
#include "atlframe.h"
#include "atlwin.h"

#include <helpers/atl-misc.h>
#include <helpers/DarkMode.h>
// CCheckBox
#include <libPPUI/wtl-pp.h>
#include <libPPUI/CDialogResizeHelper.h>

#include "header_static.h"

#include "utils.h"
#include "radio_filter_titleformat_hook.h"
#include "bookmark_core.h"
#include "bookmark_automatic.h"
#include "bookmark_list_control.h"

static const int stringlength = 256;

// preference page
static const GUID guid_bookmark_pref_page = { 0x49e82acf, 0x4954, 0x4274, { 0x80, 0xe8, 0xff, 0x74, 0xf3, 0x71, 0x1e, 0x5f } };
GUID  g_get_prefs_guid() { return guid_bookmark_pref_page; }

static const GUID guid_cfg_desc_format = { 0xa13f4068, 0xa177, 0x4cc0, { 0x9b, 0x5f, 0x4c, 0xe4, 0x85, 0x58, 0xba, 0xfc } };
static const GUID guid_cfg_date_format = { 0x25c3c9bd, 0x80b3, 0x4926, { 0xb0, 0x6, 0xed, 0x7b, 0xb9, 0x9c, 0x1f, 0x1 } };
static const GUID guid_cfg_display_ms = { 0xd7983d68, 0x7073, 0x4ae6, { 0x95, 0x1f, 0xc3, 0x46, 0xcd, 0xb, 0x3e, 0xf6 } };
static const GUID guid_cfg_autosave_newtrack_playlists = { 0x6c5226bc, 0x92a8, 0x4ae8, { 0x85, 0xde, 0xb, 0x58, 0xe3, 0x2b, 0x6e, 0x70 } };
static const GUID guid_cfg_autosave_on_quit = { 0xbce06bc, 0x1d6a, 0x4bc2, { 0xbd, 0xe1, 0x64, 0x91, 0x31, 0x9f, 0xb6, 0xcf } };
static const GUID guid_cfg_autosave_newtrack = { 0x6fc02f38, 0xfd74, 0x4352, { 0xab, 0x8f, 0xac, 0xdd, 0x10, 0x12, 0xb, 0x8f } };
static const GUID guid_cfg_autosave_focus_newtrack = { 0x612365c1, 0x8b58, 0x434f, { 0xb8, 0xb1, 0x6f, 0x72, 0x2a, 0x85, 0x21, 0xf6 } };
static const GUID guid_cfg_autosave_radio_newtrack = { 0x87cc64d0, 0x360e, 0x4c21, { 0xb9, 0x8, 0x2d, 0x94, 0x65, 0x67, 0x4a, 0xfc } };
static const GUID guid_cfg_autosave_radio_comment = { 0x9fa5bf2b, 0xb0ea, 0x416e, { 0xb7, 0x6, 0x35, 0x3c, 0x76, 0xbb, 0x99, 0xd3 } };
static const GUID guid_cfg_autosave_filter_newtrack = { 0x75728bc2, 0x6955, 0x4ea6, { 0x95, 0xe5, 0xf3, 0xb2, 0xfc, 0xef, 0x3c, 0x8c } };

static const GUID guid_cfg_verbose = { 0x354baaa6, 0x7bbb, 0x40df, { 0xbf, 0xb8, 0x8b, 0x76, 0xc, 0xbd, 0x9d, 0xd0 } };
static const GUID guid_cfg_monitor = { 0xd36b1b6d, 0x4a55, 0x48c1, { 0xab, 0x46, 0x69, 0x14, 0xb5, 0x5, 0x2f, 0x7f } };

static const GUID guid_cfg_lapse = { 0x84fb3165, 0x83eb, 0x4e17, { 0xa1, 0xa4, 0x5c, 0x6, 0x2a, 0x1c, 0x38, 0x26 } };

// {E0B79D39-269C-49ED-8892-ED46DD5F3445}
static const GUID guid_cfg_queue_flag = { 0xe0b79d39, 0x269c, 0x49ed, { 0x88, 0x92, 0xed, 0x46, 0xdd, 0x5f, 0x34, 0x45 } };

// {3B8608CE-F964-463D-9011-41999D4E0DD9}
static const GUID guid_cfg_status_flag = { 0x3b8608ce, 0xf964, 0x463d, { 0x90, 0x11, 0x41, 0x99, 0x9d, 0x4e, 0xd, 0xd9 } };

// {82C85AE9-51D4-45F4-8DBA-CE004EC45AAB}
static const GUID guid_cfg_enter_advance = { 0x82c85ae9, 0x51d4, 0x45f4, { 0x8d, 0xba, 0xce, 0x0, 0x4e, 0xc4, 0x5a, 0xab } };

// {452AC946-F849-4C79-9868-01C60F0421E6}
static const GUID guid_cfg_misc_flag = { 0x452ac946, 0xf849, 0x4c79, { 0x98, 0x68, 0x1, 0xc6, 0xf, 0x4, 0x21, 0xe6 } };

// {B73E6AAA-AFC4-4C24-BC00-85BE8371586F}
static const GUID guid_cfg_lapse_flag ={ 0xb73e6aaa, 0xafc4, 0x4c24, { 0xbc, 0x0, 0x85, 0xbe, 0x83, 0x71, 0x58, 0x6f } };

// {EC97BD7C-83B2-4E1C-BE43-0A5146C01A3A}
static const GUID guid_cfg_header_click_block_flag = { 0xec97bd7c, 0x83b2, 0x4e1c, { 0xbe, 0x43, 0xa, 0x51, 0x46, 0xc0, 0x1a, 0x3a } };

// {4DD65DEA-EDD2-47B1-A4C8-4ABAF43D64FB}
static const GUID guid_cfg_txt_filter = { 0x4dd65dea, 0xedd2, 0x47b1, { 0xa4, 0xc8, 0x4a, 0xba, 0xf4, 0x3d, 0x64, 0xfb } };
// {4DD65DEA-EDD2-47B1-A4C8-4ABAF43D64FB}
static const GUID guid_cfg_tf_filter = { 0xb1092fa6, 0x3fa0, 0x4300, { 0xa5, 0x58, 0x3c, 0x10, 0xb6, 0x2e, 0x43, 0xfe } };
// {B1092FA6-3FA0-4300-A558-3C10B62E43FE}

// defaults

static const pfc::string8 default_cfg_bookmark_desc_format = "%title% - $if2(%album% - ,- )%artist%";
static const pfc::string8 default_cfg_date_format = "%y-%m-%d %H:%M";
static const bool default_cfg_display_ms = false;
static const pfc::string8 default_cfg_autosave_newtrack_playlists = "Podcatcher";

static const bool default_cfg_autosave_newtrack = false;
static const bool default_cfg_autosave_focus_newtrack = true;
static const bool default_cfg_autosave_radio_newtrack = true;
static const bool default_cfg_autosave_radio_comment = true;
static const bool default_cfg_autosave_filter_newtrack = false;
static const bool default_cfg_autosave_on_quit = false;

static const bool default_cfg_verbose = false;
static const bool default_cfg_monitor = true;

static const pfc::string8 default_cfg_lapse = "10";

static const int default_cfg_queue_flag = 0;
static const int default_cfg_status_flag = 0;

static const bool default_cfg_edit_mode = true;

static const int default_cfg_misc_flag = 0;

static const int default_cfg_lapse_flag = LAPSE_FLAG_ENABLED;

static const pfc::string8 default_cfg_header_click_block_flag = "0";

static const pfc::string8 default_cfg_txt_filter = "Radio Classic Rock,RockClassics,Breaking News";
static const pfc::string8 default_cfg_tf_filter = "$if($or($strstr(%title%,ANEWSFM),$cont_radio_filters(%title% %artist%),$in_radio_filters(%title%)),1,0)";

// cfg_var

cfg_string cfg_desc_format(guid_cfg_desc_format, default_cfg_bookmark_desc_format.c_str());
cfg_string cfg_date_format(guid_cfg_date_format, default_cfg_date_format.c_str());
cfg_bool cfg_display_ms(guid_cfg_display_ms, default_cfg_display_ms);
cfg_string cfg_autosave_newtrack_playlists(guid_cfg_autosave_newtrack_playlists, default_cfg_autosave_newtrack_playlists.c_str());

cfg_bool cfg_autosave_newtrack(guid_cfg_autosave_newtrack, default_cfg_autosave_newtrack);
cfg_bool cfg_autosave_focus_newtrack(guid_cfg_autosave_focus_newtrack, default_cfg_autosave_focus_newtrack);
cfg_bool cfg_autosave_radio_newtrack(guid_cfg_autosave_radio_newtrack, default_cfg_autosave_radio_newtrack);
cfg_bool cfg_autosave_radio_comment(guid_cfg_autosave_radio_comment, default_cfg_autosave_radio_comment);
cfg_bool cfg_autosave_filter_newtrack(guid_cfg_autosave_filter_newtrack, default_cfg_autosave_filter_newtrack);
cfg_bool cfg_autosave_on_quit(guid_cfg_autosave_on_quit, default_cfg_autosave_on_quit);

cfg_bool cfg_verbose(guid_cfg_verbose, default_cfg_verbose);
cfg_bool cfg_monitor(guid_cfg_monitor, default_cfg_monitor);

cfg_string cfg_lapse(guid_cfg_lapse, default_cfg_lapse);

cfg_int cfg_queue_flag(guid_cfg_queue_flag, default_cfg_queue_flag);
cfg_int cfg_status_flag(guid_cfg_status_flag, default_cfg_status_flag);

cfg_bool cfg_edit_mode(guid_cfg_enter_advance, default_cfg_edit_mode);

cfg_int cfg_misc_flag(guid_cfg_misc_flag, default_cfg_misc_flag);

cfg_int cfg_lapse_flag(guid_cfg_lapse_flag, default_cfg_lapse_flag);

cfg_string cfg_header_click_block_flag(guid_cfg_header_click_block_flag, default_cfg_header_click_block_flag);

cfg_string cfg_txt_filter(guid_cfg_txt_filter, default_cfg_txt_filter.c_str());
cfg_string cfg_tf_filter(guid_cfg_tf_filter, default_cfg_tf_filter.c_str());

struct boxAndBool_t {
	int idc;
	cfg_bool* cfg;
	bool def;
};

struct boxAndInt_t {
	int idc;
	cfg_int* cfg;
	int def;
};

struct ectrlAndString_t {
	int idc;
	cfg_string* cfg;
	pfc::string8 def;
};

//snapLeft, snapTop, snapRight, snapBottom
const CDialogResizeHelper::Param resize_params[] = {
	{IDC_STATIC_PREF_HEADER, 0,0,1,0},
	{IDC_TITLEFORMAT, 0,0,1,0},
	{IDC_EDIT_AUTO_TXT_FILTER, 0,0,1,0},
	{IDC_EDIT_AUTO_TF_FILTER, 0,0,1,0},
	{IDC_AUTOSAVE_TRACK_FILTER, 0,0,1,0},
	{IDC_PREVIEW, 0,0,1,0},
	{IDC_QUEUE_FLAG, 0,0,1,0},
	{IDC_EDIT_MODE, 0,0,1,0},
	{IDC_STATUS_FLAG, 1,0,1,0},
	{IDC_BUTTON_HEADER_CB, 1,0,1,0},
	{IDC_AUTOSAVE_RADIO_TRACK, 1,0,1,0},
	{IDC_AUTOSAVE_RADIO_COMMENT_ST, 1,0,1,0},
	{IDC_LAPSE_FLAG, 1,0,1,0},
	{IDC_LAPSE, 1,0,1,0},
	{IDC_DISPLAY_MS, 1,0,1,0},
	{IDC_STATIC_DISPLAY_MS, 1,0,1,0},
	{IDC_STATIC_HEADER_LOCK, 1,0,1,0},
	{IDC_MISC_FLAG_WRITE_ON_EDITS, 1,0,1,0},
	{IDC_STATIC_DUPLICATES, 1,0,1,0},
	{IDC_MISC_FLAG_DUP_ENABLED, 1,0,1,0},
	{IDC_MISC_FLAG_DUP_REMOVE_PREV, 1,0,1,0},
	{IDC_BUTTON_AUTO_ADD_ACTIVE_PLAYLIST, 1,0,1,0},
	{IDC_VERBOSE, 1,0,1,0},
	{IDC_MONITOR, 1,0,1,0},
};

using namespace glb;

class CBookmarkPreferences : public CDialogImpl<CBookmarkPreferences>,
	public preferences_page_instance {

public:

	CBookmarkPreferences(preferences_page_callback::ptr callback) : m_callback(callback),
			m_resize_helper(resize_params) {
		//..
	}

	~CBookmarkPreferences() { 
		g_wnd_bookmark_pref = NULL;
		m_staticPrefHeader.Detach();
	}

	enum { IDD = IDD_BOOKMARK_PREFERENCES };

	t_uint32 get_state() override;
	void apply() override;
	void reset() override;

	BEGIN_MSG_MAP_EX(CBookmarkPreferences)
		CHAIN_MSG_MAP_MEMBER(m_resize_helper)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_CODE_HANDLER_EX(EN_CHANGE, OnEditChange)
		COMMAND_CODE_HANDLER_EX(CBN_SELCHANGE, OnComboChange)
		COMMAND_CODE_HANDLER_EX(BN_CLICKED, OnCheckChange)
		MESSAGE_HANDLER_SIMPLE(UMSG_NEW_TRACK, OnNewTrackMessage)
		MESSAGE_HANDLER_SIMPLE(UMSG_PAUSED, OnPaused)
	END_MSG_MAP()


private:

	BOOL OnInitDialog(CWindow, LPARAM);
	void OnEditChange(UINT uNotifyCode, int nId, CWindow wndCtl);
	void OnComboChange(UINT uNotifyCode, int nId, CWindow wndCtl);
	void OnCheckChange(UINT uNotifyCode, int nId, CWindow wndCtl);
	void on_menu_header_click_block();
	void on_add_active_playlist();

	bool HasChanged();
	void OnChanged();

	void RefreshTitleFormatResults();

	LRESULT OnNewTrackMessage() { RefreshTitleFormatResults(); return 0; }

	LRESULT OnPaused() { cfgToUi(bai_status_flag); HasChanged(); return 0; }

	// boxAndBool_t

	void cfgToUi(boxAndBool_t bab) {
		CCheckBox cb(GetDlgItem(bab.idc));
		cb.SetCheck(bab.cfg->get());
	}

	void uiToCfg(boxAndBool_t & bab) {
		CCheckBox cb(GetDlgItem(bab.idc));
		bab.cfg->set((bool)cb.GetCheck());
	}

	void defToUi(boxAndBool_t bab) {
		CCheckBox cb(GetDlgItem(bab.idc));
		cb.SetCheck(bab.def);
	}

	bool isUiChanged(boxAndBool_t bab) {
		CCheckBox cb(GetDlgItem(bab.idc));
		return bab.cfg->get() != (bool)cb.GetCheck();
	}

	// boxAndInt_t

	void cfgToUi(boxAndInt_t bai) {
		CCheckBox cb(GetDlgItem(bai.idc));
		cb.SetCheck((bool) (bai.cfg->get_value()));
	}

	void cfgToUi(boxAndInt_t bai, int flag, int idc) {
		CCheckBox cb(GetDlgItem(idc));
		cb.SetCheck(bai.cfg->get_value() & flag);
	}

	void uiToCfg(boxAndInt_t & bai) {
		CCheckBox cb(GetDlgItem(bai.idc));
		bai.cfg->set(cb.GetCheck());
	}

	void uiToCfg(boxAndInt_t& bai, int ui_fval) {
		bai.cfg->set(ui_fval);
	}

	void defToUi(boxAndInt_t bai) {
		CCheckBox cb(GetDlgItem(bai.idc));
		cb.SetCheck(bai.def);
	}

	void defToUi(boxAndInt_t bai, int flag, int idc) {
		CCheckBox cb(GetDlgItem(idc));
		cb.SetCheck(bai.def & flag);
	}

	bool isUiChanged(boxAndInt_t bai) {
		CCheckBox cb(GetDlgItem(bai.idc));
		return bai.cfg->get_value() != (int)cb.GetCheck();
	}

	bool isUiChanged(boxAndInt_t bai, int ui_fval) {
		return bai.cfg->get_value() != ui_fval;
	}

	// ectrlAndString_t

	void cfgToUi(ectrlAndString_t eat) {
		if (eat.idc == IDC_CMB_DATEFORMAT) {
			uSetDlgItemText(m_hWnd, eat.idc, eat.cfg->get_value().c_str());
		}
		else {
			uSetDlgItemText(m_hWnd, eat.idc, eat.cfg->get_value().c_str());
		}
	}

	void uiToCfg(ectrlAndString_t & eat) {
		pfc::string8 buffer;
		if (eat.idc == IDC_CMB_DATEFORMAT) {
			buffer = uGetDlgItemText(m_hWnd, eat.idc);
		}
		else {
			buffer = uGetDlgItemText(m_hWnd, eat.idc);
		}
		eat.cfg->set(buffer.c_str());
	}

	void defToUi(ectrlAndString_t eat) {
		if (eat.idc == IDC_CMB_DATEFORMAT) {
			uSetDlgItemText(m_hWnd, eat.idc, eat.def.c_str());
		}
		else {
			uSetDlgItemText(m_hWnd, eat.idc, eat.def.c_str());
		}
	}

	bool isUiChanged(ectrlAndString_t eat) {
		pfc::string8 buffer;
		if (eat.idc == IDC_CMB_DATEFORMAT) {
			buffer = uGetDlgItemText(m_hWnd, eat.idc);
		}
		else {
			buffer = uGetDlgItemText(m_hWnd, eat.idc);
		}
		return !buffer.equals(eat.cfg->get_value());
	}

private:

	CDialogResizeHelper m_resize_helper;
	HeaderStatic m_staticPrefHeader;
	fb2k::CDarkModeHooks m_dark;

	static_api_ptr_t<playback_control> m_playback_control;
	const preferences_page_callback::ptr m_callback;

	//TODO: group all these, then use for loops
	ectrlAndString_t eat_format = { IDC_TITLEFORMAT, &cfg_desc_format, default_cfg_bookmark_desc_format };
	ectrlAndString_t eat_date = { IDC_CMB_DATEFORMAT, &cfg_date_format, default_cfg_date_format };
	boxAndBool_t bab_display_ms = { IDC_DISPLAY_MS, &cfg_display_ms, default_cfg_display_ms };
	ectrlAndString_t eat_as_newtrack_playlists = { IDC_AUTOSAVE_TRACK_FILTER, &cfg_autosave_newtrack_playlists, default_cfg_autosave_newtrack_playlists };

	ectrlAndString_t eat_lapse = { IDC_LAPSE, &cfg_lapse, default_cfg_lapse };

	boxAndBool_t bab_as_newtrack = { IDC_AUTOSAVE_TRACK, &cfg_autosave_newtrack, default_cfg_autosave_newtrack };
	boxAndBool_t bab_as_focus_newtrack = { IDC_AUTOSAVE_FOCUS_TRACK, &cfg_autosave_focus_newtrack, default_cfg_autosave_focus_newtrack };
	boxAndBool_t bab_as_radio_newtrack = { IDC_AUTOSAVE_RADIO_TRACK, &cfg_autosave_radio_newtrack, default_cfg_autosave_radio_newtrack };
	boxAndBool_t bab_as_radio_comment = { IDC_AUTOSAVE_RADIO_COMMENT_ST, &cfg_autosave_radio_comment, default_cfg_autosave_radio_comment };
	boxAndBool_t bab_as_filter_newtrack = { IDC_AUTOSAVE_TRACK_FILTER_CHECK, &cfg_autosave_filter_newtrack, default_cfg_autosave_filter_newtrack };
	boxAndBool_t bab_as_exit = { IDC_AUTOSAVE_EXIT, &cfg_autosave_on_quit, default_cfg_autosave_on_quit };

	boxAndBool_t bab_verbose = { IDC_VERBOSE, &cfg_verbose, default_cfg_verbose };
	boxAndBool_t bab_monitor = { IDC_MONITOR, &cfg_monitor, default_cfg_monitor };

	boxAndInt_t bai_queue_flag = { IDC_QUEUE_FLAG, &cfg_queue_flag, default_cfg_queue_flag };
	boxAndInt_t bai_status_flag = { IDC_STATUS_FLAG, &cfg_status_flag, default_cfg_status_flag };

	boxAndBool_t bab_edit_mode = { IDC_EDIT_MODE, &cfg_edit_mode, default_cfg_edit_mode };

	boxAndInt_t bai_misc_flag = { IDC_MISC_FLAG_ENTER_KEY_DOWN, &cfg_misc_flag, default_cfg_misc_flag };

	boxAndInt_t bai_lapse_flag = { IDC_LAPSE_FLAG, &cfg_lapse_flag, default_cfg_lapse_flag };

	ectrlAndString_t eat_header_click_block_flag = { IDC_HIDDEN_HEADER_CLICK_BLOCK_FLAG, &cfg_header_click_block_flag, default_cfg_header_click_block_flag };
	ectrlAndString_t eat_txt_filter = { IDC_EDIT_AUTO_TXT_FILTER, &cfg_txt_filter, default_cfg_txt_filter };
	ectrlAndString_t eat_tf_filter = { IDC_EDIT_AUTO_TF_FILTER, &cfg_tf_filter, default_cfg_tf_filter };
};

void ConvertString8(const pfc::string8 orig, wchar_t* out, size_t max) {
	pfc::stringcvt::convert_utf8_to_wide(out, max, orig.get_ptr(), orig.length());
}

void InitDateCombo(HWND hwndParent, UINT idc_date, pfc::string8 strval) {

	std::vector<std::string> vfd = {
		"%a %b %d %H:%M:%S %Y",
		"%y-%m-%d %H:%M:%S %a",
		"%y-%m-%d %H:%M %a",
		"%y-%m-%d %H:%M"
	};

	CComboBox cmb = GetDlgItem(hwndParent, idc_date);

	for (auto& w : vfd) {
		WCHAR wstr[DATE_BUFFER_SIZE];
		ConvertString8(w.c_str(), wstr, DATE_BUFFER_SIZE - 1);
		cmb.SetItemData(cmb.AddString(wstr), cmb.GetCount());
	}

	auto cursel = std::find(vfd.begin(), vfd.end(), strval.c_str());
	auto curndx = std::distance(vfd.begin(), cursel);

	for (int i = 0; i < cmb.GetCount(); ++i) {
		WCHAR wstr[DATE_BUFFER_SIZE];
		ConvertString8(strval.c_str(), wstr, DATE_BUFFER_SIZE - 1);
		auto dbg = cmb.GetCurSel();
		if (cmb.GetItemData(i) == curndx)
		{
			cmb.SetCurSel(i);
			break;
		}
	}
}

BOOL CBookmarkPreferences::OnInitDialog(CWindow, LPARAM) {

	g_wnd_bookmark_pref = m_hWnd;

	InitDateCombo(m_hWnd, eat_date.idc, eat_date.cfg->get_value());

	cfgToUi(eat_format);
	cfgToUi(eat_date);
	cfgToUi(bab_display_ms);
	cfgToUi(eat_as_newtrack_playlists);

	cfgToUi(eat_lapse);

	cfgToUi(bab_as_exit);
	cfgToUi(bab_as_newtrack);
	cfgToUi(bab_as_focus_newtrack);
	cfgToUi(bab_as_radio_newtrack);
	cfgToUi(bab_as_radio_comment);
	cfgToUi(bab_as_filter_newtrack);

	cfgToUi(bab_verbose);
	cfgToUi(bab_monitor);

	cfgToUi(bai_queue_flag, QUEUE_RESTORE_TO_FLAG, /*IDC_QUEUE_FLAG*/bai_queue_flag.idc);
	cfgToUi(bai_queue_flag, QUEUE_FLUSH_FLAG, IDC_QUEUE_FLUSH_FLAG);

	cfgToUi(bai_status_flag);

	cfgToUi(bab_edit_mode);

	cfgToUi(bai_misc_flag, MISC_FLAG_EDIT_ENTER_KEY_ADV, bai_misc_flag.idc);
	cfgToUi(bai_misc_flag, MISC_FLAG_INSTANT_WRITE_ON_EDITS, IDC_MISC_FLAG_WRITE_ON_EDITS);
	cfgToUi(bai_misc_flag, MISC_DUP_ENABLED_FLAG, IDC_MISC_FLAG_DUP_ENABLED);
	cfgToUi(bai_misc_flag, MISC_DUP_REMOVE_PREV_FLAG, IDC_MISC_FLAG_DUP_REMOVE_PREV);

	cfgToUi(bai_lapse_flag, LAPSE_FLAG_ENABLED, IDC_LAPSE_FLAG);

	cfgToUi(eat_header_click_block_flag);

	cfgToUi(eat_txt_filter);
	cfgToUi(eat_tf_filter);

	//static header

	HWND wndStaticHeader = uGetDlgItem(IDC_STATIC_PREF_HEADER);
	m_staticPrefHeader.SubclassWindow(wndStaticHeader);
	m_staticPrefHeader.PaintHeader();

	//dark mode
	m_dark.AddDialogWithControls(*this);

	RefreshTitleFormatResults();

	return TRUE;
}

void CBookmarkPreferences::OnEditChange(UINT uNotifyCode, int nId, CWindow wndCtl) {

	OnChanged();
}

void CBookmarkPreferences::OnComboChange(UINT uNotifyCode, int nId, CWindow wndCtl) {
	if (nId != IDC_CMB_DATEFORMAT) {
		//nothing to do
		return;
	}
	pfc::string8 strFormat = uGetDlgItemText(m_hWnd, nId);

	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	auto sctime = asctime(&tm);

	char buffer[DATE_BUFFER_SIZE];
	std::strftime(buffer, DATE_BUFFER_SIZE, strFormat, &tm);

	WCHAR wstr[stringlength];
	ConvertString8(buffer, wstr, stringlength - 1);

	SetDlgItemTextW(IDC_PREVIEW_DATE_FORMAT, wstr);
	m_callback->on_state_changed();
}

void CBookmarkPreferences::on_add_active_playlist() {

	size_t act_index = playlist_manager::get()->get_active_playlist();
	if (act_index == SIZE_MAX) { return; }

	pfc::string newName;
	playlist_manager::get()->playlist_get_name(act_index, newName);

	//replace all commas with dots (because of the comma-seperated list)
	newName.replace_char(',', '.');

	pfc::string8 curr_filter;
	uGetDlgItemText(m_hWnd, IDC_AUTOSAVE_TRACK_FILTER, curr_filter);
	//check if name already exists
	std::stringstream ss(curr_filter.c_str());
	std::string token;
	while (std::getline(ss, token, ',')) {
		if (!stricmp_utf8(token.c_str(), newName.c_str())) {
			//skip
			return;
		}
	}

	FB2K_console_print_v("Adding to auto-bookmarking playlists: ", newName);

	//Add newName to the ui:
	wchar_t fieldContent[1 + (stringlength * 2)];
	GetDlgItemTextW(IDC_AUTOSAVE_TRACK_FILTER, (LPTSTR)fieldContent, stringlength);

	if (fieldContent[0] != L"\0"[0]) {
		wcscat_s(fieldContent, L",");
	}

	WCHAR wstr[1024];
	ConvertString8(newName, wstr, 1024 - 1);

	wcscat_s(fieldContent, wstr);

	SetDlgItemText(IDC_AUTOSAVE_TRACK_FILTER, fieldContent);

}

void CBookmarkPreferences::on_menu_header_click_block() {

	CRect rcButton;
	HWND hwndCtrl = ::GetDlgItem(m_hWnd, IDC_BUTTON_HEADER_CB);
	::GetWindowRect(hwndCtrl, rcButton);

	POINT pt = {};
	pt.x = rcButton.left;
	pt.y = rcButton.bottom;

	pfc::string8 currStrFlag = uGetDlgItemText(m_hWnd, IDC_HIDDEN_HEADER_CLICK_BLOCK_FLAG);
	int tmpFlag = atoi(currStrFlag);

	enum { CMD_1 = 1, CMD_6 = 6, CMD_ALL, CMD_NONE };
	//00111111
	const unsigned int tmpFlagAll = (1u << CMD_6) - 1;

	HMENU hSplitMenu = CreatePopupMenu();

	AppendMenu(hSplitMenu, MF_STRING | (tmpFlag & (1 << 0) ? MF_CHECKED : MF_UNCHECKED), CMD_1, L"#");
	AppendMenu(hSplitMenu, MF_STRING | (tmpFlag & (1 << 1) ? MF_CHECKED : MF_UNCHECKED), CMD_1 + 1, L"Time");
	AppendMenu(hSplitMenu, MF_STRING | (tmpFlag & (1 << 2) ? MF_CHECKED : MF_UNCHECKED), CMD_1 + 2, L"Bookmark");
	AppendMenu(hSplitMenu, MF_STRING | (tmpFlag & (1 << 3) ? MF_CHECKED : MF_UNCHECKED), CMD_1 + 3, L"Playlist");
	AppendMenu(hSplitMenu, MF_STRING | (tmpFlag & (1 << 4) ? MF_CHECKED : MF_UNCHECKED), CMD_1 + 4, L"Comment");
	AppendMenu(hSplitMenu, MF_STRING | (tmpFlag & (1 << 5) ? MF_CHECKED : MF_UNCHECKED), CMD_6, L"Date");
	AppendMenu(hSplitMenu, MF_SEPARATOR, 0, 0);
	AppendMenu(hSplitMenu, MF_STRING | (tmpFlag == tmpFlagAll ? MF_CHECKED : MF_UNCHECKED), CMD_ALL, L"All");
	AppendMenu(hSplitMenu, MF_STRING | (tmpFlag == 0 ? MF_CHECKED : MF_UNCHECKED), CMD_NONE, L"None");

	int cmd = TrackPopupMenu(hSplitMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, pt.x, pt.y, 0, m_hWnd, NULL);
	DestroyMenu(hSplitMenu);

	if (!cmd) return;

	int cmd_ndx = cmd - 1;

	if (cmd_ndx >= 0 && cmd_ndx < 6) {
		//tmpFlag = tmpFlag & (1 << cmd_ndx) ? tmpFlag - (1 << cmd_ndx) : tmpFlag + (1 << cmd_ndx);
		if (tmpFlag & (1 << cmd_ndx))
			tmpFlag &= ~(1 << cmd_ndx);
		else
			tmpFlag |= (1 << cmd_ndx);
	}
	else if (cmd == CMD_ALL) {
		tmpFlag = tmpFlagAll;
	}
	else if (cmd == CMD_NONE) {
		tmpFlag = 0;
	}

	uSetDlgItemText(m_hWnd, IDC_HIDDEN_HEADER_CLICK_BLOCK_FLAG, std::to_string(tmpFlag).c_str());
	OnChanged();

}

void CBookmarkPreferences::OnCheckChange(UINT uNotifyCode, int nId, CWindow wndCtl) {

	if (nId == IDC_BUTTON_HEADER_CB) {
		on_menu_header_click_block();
		OnChanged();
	}
	else if (nId == IDC_BUTTON_AUTO_ADD_ACTIVE_PLAYLIST) {
		on_add_active_playlist();
		OnChanged();
	}
	else {

		OnChanged();
	}
}

t_uint32 CBookmarkPreferences::get_state() {

	t_uint32 state = preferences_state::resettable | preferences_state::dark_mode_supported;

	if (HasChanged()) state |= preferences_state::changed;
	return state;
}

void CBookmarkPreferences::reset() {

	defToUi(eat_format);
	defToUi(eat_date);
	defToUi(bab_display_ms);
	defToUi(eat_as_newtrack_playlists);

	defToUi(eat_lapse);

	defToUi(bab_as_exit);
	defToUi(bab_as_newtrack);
	defToUi(bab_as_focus_newtrack);
	defToUi(bab_as_radio_newtrack);
	defToUi(bab_as_radio_comment);
	defToUi(bab_as_filter_newtrack);

	defToUi(bab_verbose);
	defToUi(bab_monitor);

	defToUi(bai_queue_flag, QUEUE_RESTORE_TO_FLAG, /*IDC_QUEUE_FLAG*/bai_queue_flag.idc);
	defToUi(bai_queue_flag, QUEUE_FLUSH_FLAG, IDC_QUEUE_FLUSH_FLAG);

	defToUi(bai_status_flag);

	defToUi(bab_edit_mode);

	defToUi(bai_misc_flag, MISC_FLAG_EDIT_ENTER_KEY_ADV, /*IDC_MISC_FLAG_ENTER_KEY_DOWN*/bai_misc_flag.idc);
	defToUi(bai_misc_flag, MISC_FLAG_INSTANT_WRITE_ON_EDITS, IDC_MISC_FLAG_WRITE_ON_EDITS);
	defToUi(bai_misc_flag, MISC_DUP_ENABLED_FLAG, IDC_MISC_FLAG_DUP_ENABLED);
	defToUi(bai_misc_flag, MISC_DUP_REMOVE_PREV_FLAG, IDC_MISC_FLAG_DUP_REMOVE_PREV);

	defToUi(bai_lapse_flag, LAPSE_FLAG_ENABLED, IDC_LAPSE_FLAG);

	defToUi(eat_header_click_block_flag);

	defToUi(eat_txt_filter);
	defToUi(eat_tf_filter);

	OnChanged();
}

void CBookmarkPreferences::apply() {

	bool bneedReload = isUiChanged(bab_display_ms);

	uiToCfg(eat_format);
	uiToCfg(eat_date);
	uiToCfg(bab_display_ms);
	uiToCfg(eat_as_newtrack_playlists);

	pfc::string8 buffer;
	buffer = uGetDlgItemText(m_hWnd, eat_lapse.idc);
	if (atoi(buffer) >= 1 && atoi(buffer) <= 60) {
		uiToCfg(eat_lapse);
	}
	else {
		cfgToUi(eat_lapse);
	}

	uiToCfg(bab_as_exit);
	uiToCfg(bab_as_newtrack);
	uiToCfg(bab_as_focus_newtrack);
	uiToCfg(bab_as_radio_newtrack);
	uiToCfg(bab_as_radio_comment);
	uiToCfg(bab_as_filter_newtrack);

	//refresh dummy
	if (bab_as_newtrack.cfg->get()) {
		g_bmAuto.updateDummy();
	}

	uiToCfg(bab_as_filter_newtrack);
	uiToCfg(bab_verbose);
	uiToCfg(bab_monitor);

	int ui_fval = 0;
	ui_fval = IsDlgButtonChecked(/*IDC_QUEUE_FLAG*/bai_queue_flag.idc) ? QUEUE_RESTORE_TO_FLAG : ui_fval;
	ui_fval = IsDlgButtonChecked(IDC_QUEUE_FLUSH_FLAG) ? ui_fval | QUEUE_FLUSH_FLAG : ui_fval;
	uiToCfg(bai_queue_flag, ui_fval);

	uiToCfg(bai_status_flag);
	uiToCfg(bab_edit_mode);

	ui_fval = 0;
	ui_fval = IsDlgButtonChecked(/*IDC_MISC_FLAG_ENTER_KEY_DOWN*/bai_misc_flag.idc) ? MISC_FLAG_EDIT_ENTER_KEY_ADV : ui_fval;
	ui_fval = IsDlgButtonChecked(IDC_MISC_FLAG_WRITE_ON_EDITS) ? ui_fval | MISC_FLAG_INSTANT_WRITE_ON_EDITS : ui_fval;
	ui_fval = IsDlgButtonChecked(IDC_MISC_FLAG_DUP_ENABLED) ? ui_fval | MISC_DUP_ENABLED_FLAG : ui_fval;
	ui_fval = IsDlgButtonChecked(IDC_MISC_FLAG_DUP_REMOVE_PREV) ? ui_fval | MISC_DUP_REMOVE_PREV_FLAG : ui_fval;
	uiToCfg(bai_misc_flag, ui_fval);

	ui_fval = 0;
	ui_fval = IsDlgButtonChecked(IDC_LAPSE_FLAG) ? ui_fval | LAPSE_FLAG_ENABLED: ui_fval;
	uiToCfg(bai_lapse_flag, ui_fval);

	uiToCfg(eat_header_click_block_flag);

	uiToCfg(eat_txt_filter);
	uiToCfg(eat_tf_filter);

	if (bneedReload) {
		for (auto gui : g_guiLists) {
			gui->ReloadItems(bit_array_true());
		}
	}

	RefreshTitleFormatResults();
	OnChanged();
}

bool CBookmarkPreferences::HasChanged() {

	bool result = false;
	result |= isUiChanged(eat_format);
	result |= isUiChanged(eat_date);
	result |= isUiChanged(bab_display_ms);
	result |= isUiChanged(eat_as_newtrack_playlists);

	result |= isUiChanged(eat_lapse);

	result |= isUiChanged(bab_as_exit);
	result |= isUiChanged(bab_as_newtrack);
	result |= isUiChanged(bab_as_focus_newtrack);
	result |= isUiChanged(bab_as_radio_newtrack);
	result |= isUiChanged(bab_as_radio_comment);
	result |= isUiChanged(bab_as_filter_newtrack);

	result |= isUiChanged(bab_verbose);
	result |= isUiChanged(bab_monitor);

	int ui_fval = 0;
	ui_fval = IsDlgButtonChecked(/*IDC_QUEUE_FLAG*/bai_queue_flag.idc) ? QUEUE_RESTORE_TO_FLAG : ui_fval;
	ui_fval = IsDlgButtonChecked(IDC_QUEUE_FLUSH_FLAG) ? ui_fval | QUEUE_FLUSH_FLAG : ui_fval;

	result |= isUiChanged(bai_queue_flag, ui_fval);

	result |= isUiChanged(bai_status_flag);

	result |= isUiChanged(bab_edit_mode);

	ui_fval = 0;
	ui_fval = IsDlgButtonChecked(/*IDC_MISC_FLAG_ENTER_KEY_DOWN*/bai_misc_flag.idc) ? MISC_FLAG_EDIT_ENTER_KEY_ADV : ui_fval;
	ui_fval = IsDlgButtonChecked(IDC_MISC_FLAG_WRITE_ON_EDITS) ? ui_fval | MISC_FLAG_INSTANT_WRITE_ON_EDITS : ui_fval;
	ui_fval = IsDlgButtonChecked(IDC_MISC_FLAG_DUP_ENABLED) ? ui_fval | MISC_DUP_ENABLED_FLAG : ui_fval;
	ui_fval = IsDlgButtonChecked(IDC_MISC_FLAG_DUP_REMOVE_PREV) ? ui_fval | MISC_DUP_REMOVE_PREV_FLAG : ui_fval;

	result |= isUiChanged(bai_misc_flag, ui_fval);

	ui_fval = 0;
	ui_fval = IsDlgButtonChecked(IDC_LAPSE_FLAG) ? ui_fval | LAPSE_FLAG_ENABLED : ui_fval;

	result |= isUiChanged(bai_lapse_flag, ui_fval);

	result |= isUiChanged(eat_header_click_block_flag);

	result |= isUiChanged(eat_txt_filter);
	result |= isUiChanged(eat_tf_filter);
	return result;
}

namespace fltr = filters;

void CBookmarkPreferences::RefreshTitleFormatResults() {

	//todo: rev apply not needed
	//      once desc title format is modded it runs on each keystroke
	pfc::string8 titleformat = uGetDlgItemText(m_hWnd, IDC_TITLEFORMAT);
	pfc::string8 radio_filters = uGetDlgItemText(m_hWnd, IDC_EDIT_AUTO_TXT_FILTER);
	pfc::string8 tf_filter = uGetDlgItemText(m_hWnd, IDC_EDIT_AUTO_TF_FILTER);

	titleformat_object::ptr p_script;
	static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(p_script, titleformat);

	pfc::string_formatter songDesc;
	bookmark_t bm = g_bmAuto.getDummy();

	bool playback_ok = false;

	if (bm.isRadio()) {

		pfc::string8 test_songDesc;
		titleformat_object::ptr p_test_script;
		static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(p_script, "%title%");
		bool test_playback_ok = m_playback_control->playback_format_title(NULL, test_songDesc, p_script, NULL, playback_control::display_level_all);

		filters::radio_nfo_type rnt;
		filters::get_radio_nfo(test_songDesc, rnt);

		radio_filter_titleformat_hook ra_hook;
		std::vector<pfc::string8>vfilters;

		fltr::get_filters(radio_filters, vfilters);
		ra_hook.setData(g_bmAuto.getDummy().get_fdn(), vfilters);

		playback_ok = parse_radio_info(rnt, &ra_hook, songDesc, titleformat) != SIZE_MAX;

		if (playback_ok) {


			titleformat_object::ptr tfo_filter;
			static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(tfo_filter, cfg_tf_filter.get_value());

			pfc::string filter_res;
			bool check_pass = playback_control::get()->playback_format_title(&ra_hook, filter_res, tfo_filter, NULL, playback_control::display_level_all);

			if (check_pass)
			{
				if (pfc::string_is_numeric(filter_res)) {
					if (atoi(filter_res) != 0) {
						songDesc = PFC_string_formatter() << "(filtered) " << songDesc;
					}
				}
				else {

					pfc::string8 tmpstr;
					if (std::find_if(vfilters.begin(), vfilters.end(), [filter_res](const pfc::string8 s)
						{ return s.equals(filter_res); }) != vfilters.end()) {
						songDesc = PFC_string_formatter() << "(filtered match) " << songDesc;
					}
				}
			}
		}
	}

	if (!playback_ok)
	{
		if (!m_playback_control->playback_format_title(NULL, songDesc, p_script, NULL, playback_control::display_level_all)) {
			songDesc << "(resume playback to generate track description)";
			return;
		}
	}

	const pfc::stringcvt::string_os_from_utf8 os_tag_name(songDesc);
	SetDlgItemTextW(IDC_PREVIEW, os_tag_name);
	OnComboChange(0, IDC_CMB_DATEFORMAT, NULL);
}

void CBookmarkPreferences::OnChanged() {

	bool changed_desc_tf = isUiChanged(eat_format);

	if (changed_desc_tf) {
		RefreshTitleFormatResults();
	}

	//enable/disable the apply button
	m_callback->on_state_changed();
}

class preferences_page_myimpl : public preferences_page_impl<CBookmarkPreferences> {

public:

	const char * get_name() override { return COMPONENT_NAME_HC; }
	GUID get_guid() override { return guid_bookmark_pref_page; }
	GUID get_parent_guid() override { return guid_tools; }
};

static preferences_page_factory_t<preferences_page_myimpl> g_preferences_page_myimpl_factory;