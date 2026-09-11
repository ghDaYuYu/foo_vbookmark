#include "stdafx.h"

#include "bookmark_core.h";
#include "bookmark_list_dialog.h"
#include "radio_filter_titleformat_hook.h"
#include "utils.h"
#include "bookmark_preferences.h"

using namespace glb;

static t_uint32 g_current_tab;
static HWND g_hWndTabDialog[NUM_TABS] = {nullptr};
static HWND g_hWndCurrentTab = nullptr;

static const int kStringLength = 256;

class CBookmarkPreferences;

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
static const GUID guid_cfg_edit_mode = { 0x82c85ae9, 0x51d4, 0x45f4, { 0x8d, 0xba, 0xce, 0x0, 0x4e, 0xc4, 0x5a, 0xab } };
// {452AC946-F849-4C79-9868-01C60F0421E6}
static const GUID guid_cfg_misc_flag = { 0x452ac946, 0xf849, 0x4c79, { 0x98, 0x68, 0x1, 0xc6, 0xf, 0x4, 0x21, 0xe6 } };
// {B73E6AAA-AFC4-4C24-BC00-85BE8371586F}
static const GUID guid_cfg_lapse_flag = { 0xb73e6aaa, 0xafc4, 0x4c24, { 0xbc, 0x0, 0x85, 0xbe, 0x83, 0x71, 0x58, 0x6f } };
// {EC97BD7C-83B2-4E1C-BE43-0A5146C01A3A}
static const GUID guid_cfg_header_click_block_flag = { 0xec97bd7c, 0x83b2, 0x4e1c, { 0xbe, 0x43, 0xa, 0x51, 0x46, 0xc0, 0x1a, 0x3a } };
// {4DD65DEA-EDD2-47B1-A4C8-4ABAF43D64FB}
static const GUID guid_cfg_txt_filter = { 0x4dd65dea, 0xedd2, 0x47b1, { 0xa4, 0xc8, 0x4a, 0xba, 0xf4, 0x3d, 0x64, 0xfb } };
// {B1092FA6-3FA0-4300-A558-3C10B62E43FE}
static const GUID guid_cfg_tf_filter = { 0xb1092fa6, 0x3fa0, 0x4300, { 0xa5, 0x58, 0x3c, 0x10, 0xb6, 0x2e, 0x43, 0xfe } };
// {E5864BB5-774E-4466-8C4D-E1CA9A6AC43D}
static const GUID guid_cfg_rq_wait =
{ 0xe5864bb5, 0x774e, 0x4466, { 0x8c, 0x4d, 0xe1, 0xca, 0x9a, 0x6a, 0xc4, 0x3d } };
// {F8C7F643-DFE5-4436-BFF3-32ABD93B9116}
static const GUID guid_cfg_last_tab = { 0xf8c7f643, 0xdfe5, 0x4436, { 0xbf, 0xf3, 0x32, 0xab, 0xd9, 0x3b, 0x91, 0x16 } };

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

cfg_bool cfg_edit_mode(guid_cfg_edit_mode, default_cfg_edit_mode);

cfg_int cfg_misc_flag(guid_cfg_misc_flag, default_cfg_misc_flag);
cfg_int cfg_lapse_flag(guid_cfg_lapse_flag, default_cfg_lapse_flag);

cfg_string cfg_header_click_block_flag(guid_cfg_header_click_block_flag, default_cfg_header_click_block_flag);

cfg_string cfg_txt_filter(guid_cfg_txt_filter, default_cfg_txt_filter.c_str());
cfg_string cfg_tf_filter(guid_cfg_tf_filter, default_cfg_tf_filter.c_str());

cfg_string cfg_rq_wait(guid_cfg_rq_wait, default_cfg_rq_wait);

cfg_int cfg_last_tab(guid_cfg_last_tab, default_cfg_last_tab);

void CBookmarkPreferences::InitTabs() {
	tab_table.append_single(tab_entry("General", config_0_dialog_proc, IDD_DIALOG_CONF_0));
	tab_table.append_single(tab_entry("Filters", config_1_dialog_proc, IDD_DIALOG_CONF_1));
	tab_table.append_single(tab_entry("Other", config_2_dialog_proc, IDD_DIALOG_CONF_2));
}

CBookmarkPreferences::~CBookmarkPreferences() {

	if (glb::configuration_dialog) {
		glb::configuration_dialog = nullptr;

	}
}

//from libPPUI\CDialogResizeHelper.cpp
static BOOL GetChildWindowRect(HWND wnd, UINT id, RECT* child)
{
	RECT temp;
	HWND wndChild = GetDlgItem(wnd, id);
	if (wndChild == NULL) return FALSE;
	if (!GetWindowRect(wndChild, &temp)) return FALSE;
	if (!MapWindowPoints(0, wnd, (POINT*)&temp, 2)) return FALSE;
	*child = temp;
	return TRUE;
}

LRESULT CBookmarkPreferences::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {

	glb::g_wnd_bookmark_pref = m_hWnd;

	InitTabs();

	HWND hWndTab = uGetDlgItem(IDC_TAB_CFG);

	m_dark.AddDialog(m_hWnd);
	m_dark.AddTabCtrl(hWndTab);

	// set up tabs and create (not visible) subdialogs
	uTCITEM item = {0};
	item.mask = TCIF_TEXT;
	for (size_t n = 0; n < NUM_TABS; n++) {
		PFC_ASSERT(tab_table[n].m_pszName != nullptr);

		item.pszText = tab_table[n].m_pszName;
		uTabCtrl_InsertItem(hWndTab, n, &item);

		g_hWndTabDialog[n] = tab_table[n].CreateTabDialog(m_hWnd, (LPARAM)this);

		// darkmode
		m_dark.AddDialog(g_hWndTabDialog[n]);
	}

	// get the size of the inner part of the tab control
	RECT rcTab;
	GetChildWindowRect(m_hWnd, IDC_TAB_CFG, &rcTab);
	uSendMessage(hWndTab, TCM_ADJUSTRECT, FALSE, (LPARAM)&rcTab);

	// tab Control
	RECT rcTabDialog;
	::GetClientRect(g_hWndTabDialog[0], &rcTabDialog);
	OffsetRect(&rcTabDialog, rcTab.left, rcTab.top);
	rcTabDialog.bottom = (rcTabDialog.bottom > rcTab.bottom)
		? rcTabDialog.bottom : rcTab.bottom;
	rcTabDialog.right = (rcTabDialog.right > rcTab.right)
		? rcTabDialog.right : rcTab.right;

	uSendMessage(hWndTab, TCM_ADJUSTRECT, TRUE, (LPARAM)&rcTabDialog);

	::SetWindowPos(hWndTab, nullptr,
		rcTabDialog.left, rcTabDialog.top,
		rcTabDialog.right - rcTabDialog.left, rcTabDialog.bottom - rcTabDialog.top,
		SWP_NOZORDER | SWP_NOACTIVATE);

	// position the subdialogs in the inner part of the tab control
	uSendMessage(hWndTab, TCM_ADJUSTRECT, FALSE, (LPARAM)&rcTabDialog);

	// fix left white stripe
	if (!m_dark.IsDark()) {
		InflateRect(&rcTabDialog, 2, 1);
		OffsetRect(&rcTabDialog, -1, 1);
	}

	for (size_t n = 0; n < tabsize(g_hWndTabDialog); n++) {
		if (g_hWndTabDialog[n] != nullptr) {
			::SetWindowPos(g_hWndTabDialog[n], nullptr,
				rcTabDialog.left, rcTabDialog.top,
				rcTabDialog.right - rcTabDialog.left, rcTabDialog.bottom - rcTabDialog.top,
				SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	g_current_tab = static_cast<t_uint32>(cfg_last_tab.get());
	uSendMessage(hWndTab, TCM_SETCURSEL, g_current_tab, 0);

	g_hWndCurrentTab = g_hWndTabDialog[g_current_tab];
	if (g_hWndCurrentTab) {
		::ShowWindow(g_hWndCurrentTab, SW_SHOW);
	}

	return TRUE;
}

void CBookmarkPreferences::OnEditChange(UINT uNotifyCode, int nId, CWindow wndCtl) {
	OnChanged();
}

void ConvertString8(const pfc::string8 orig, wchar_t* out, size_t max) {
	pfc::stringcvt::convert_utf8_to_wide(out, max, orig.get_ptr(), orig.length());
}

void CBookmarkPreferences::OnComboChange(UINT /*uNotifyCode*/, int nId, CWindow /*wndCtl*/) {

	if (nId != IDC_CMB_DATEFORMAT) {
		//nothing to do
		return;
	}
	pfc::string8 strFormat = uGetDlgItemText(g_hWndCurrentTab, nId);

	auto t = std::time(nullptr);
#pragma warning( push )
#pragma warning( disable : 4996 )
	auto tm = *std::localtime(&t);
	auto sctime = asctime(&tm);
#pragma warning( pop )

	char buffer[DATE_BUFFER_SIZE];
	std::strftime(buffer, DATE_BUFFER_SIZE, strFormat, &tm);

	WCHAR wstr[kStringLength];
	ConvertString8(buffer, wstr, kStringLength - 1);
	::SetDlgItemTextW(g_hWndCurrentTab, IDC_PREVIEW_DATE_FORMAT, wstr);

	m_callback->on_state_changed();
}

void CBookmarkPreferences::init_current_tab() {

	if (g_hWndCurrentTab == g_hWndTabDialog[CONF_0_TAB]) {
		init_config_0_dialog(g_hWndTabDialog[CONF_0_TAB], false);
	}
	else if (g_hWndCurrentTab == g_hWndTabDialog[CONF_1_TAB]) {
		init_config_1_dialog(g_hWndTabDialog[CONF_1_TAB], false);
	}
	else if (g_hWndCurrentTab == g_hWndTabDialog[CONF_2_TAB]) {
		init_config_2_dialog(g_hWndTabDialog[CONF_2_TAB], false);
	}
}

LRESULT CBookmarkPreferences::OnChangingTab(WORD /*wNotifyCode*/, LPNMHDR /*lParam*/, BOOL& /*bHandled*/) {

	if (get_state() & preferences_state::changed) {

		CYesNoApiDialog yndlg;
		auto res = yndlg.query(m_hWnd, { "Configuration Changes","Apply Changes ?" }, true, false);

		switch (res) {
		case 1:
			pushcfg(false);
			OnChanged();
			break;
		case 2:
			setting_dlg = true;
			init_current_tab();
			setting_dlg = false;
			OnChanged();
			break;
		}
	}
	return FALSE;
}

LRESULT CBookmarkPreferences::OnChangeTab(WORD /*wNotifyCode*/, LPNMHDR /*lParam*/, BOOL& /*bHandled*/) {

	pfc::string8 np_preview;
	//backup current
	if (g_current_tab == 0 || g_current_tab == 1) {
		np_preview = uGetDlgItemText(g_hWndCurrentTab, IDC_PREVIEW);
	}
	
	if (g_hWndCurrentTab != nullptr) {
		::ShowWindow(g_hWndCurrentTab, SW_HIDE);
	}

	g_hWndCurrentTab = nullptr;

	g_current_tab = (t_uint32)::SendDlgItemMessage(m_hWnd, IDC_TAB_CFG, TCM_GETCURSEL, 0, 0);

	if (g_current_tab < tabsize(g_hWndTabDialog)) {

		g_hWndCurrentTab = g_hWndTabDialog[g_current_tab];
		cfg_last_tab.set(g_current_tab);
		//restore into the new active tab
		if (g_current_tab == 0 || g_current_tab == 1 && np_preview.get_length()) {
			uSetDlgItemText(g_hWndCurrentTab, IDC_PREVIEW, np_preview);
		}

		::ShowWindow(g_hWndCurrentTab, SW_SHOW);
	}
	return FALSE;
}

bool CBookmarkPreferences::build_current_cfg(bool reset) {

	bool bres = false;

	if (reset || g_hWndCurrentTab == g_hWndTabDialog[CONF_0_TAB]) {
		save_config_0_dialog(g_hWndTabDialog[CONF_0_TAB], !reset);
	}
	if (reset || g_hWndCurrentTab == g_hWndTabDialog[CONF_1_TAB]) {
		save_config_1_dialog(g_hWndTabDialog[CONF_1_TAB], !reset);
	}
	if (reset || g_hWndCurrentTab == g_hWndTabDialog[CONF_2_TAB]) {
		save_config_2_dialog(g_hWndTabDialog[CONF_2_TAB], !reset);
	}

	bres = reset || HasChanged();
	return bres;
}

void CBookmarkPreferences::pushcfg(bool reset) {

	if (build_current_cfg(reset)) {

	}
}

void CBookmarkPreferences::reset() {

	BOOL bDummy;
	OnDefaults(0, 0, NULL, bDummy);
}

LRESULT CBookmarkPreferences::OnDefaults(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {

	CYesNoApiDialog yndlg;

	if (!yndlg.query(m_hWnd, /*true,*/ { "Reset", "Reset Discogger settings?" })) {
		return FALSE;
	}

	cfg_desc_format = default_cfg_bookmark_desc_format;
	cfg_date_format = default_cfg_date_format;
	cfg_display_ms =  default_cfg_display_ms;
	cfg_autosave_newtrack_playlists = default_cfg_autosave_newtrack_playlists.c_str();

	cfg_autosave_newtrack = default_cfg_autosave_newtrack;
	cfg_autosave_focus_newtrack = default_cfg_autosave_focus_newtrack;
	cfg_autosave_radio_newtrack = default_cfg_autosave_radio_newtrack;
	cfg_autosave_radio_comment = default_cfg_autosave_radio_comment;
	cfg_autosave_filter_newtrack = default_cfg_autosave_filter_newtrack;
	cfg_autosave_on_quit = default_cfg_autosave_on_quit;

	cfg_verbose = default_cfg_verbose;
	cfg_monitor = default_cfg_monitor;

	cfg_lapse = default_cfg_lapse;

	cfg_queue_flag = default_cfg_queue_flag;
	cfg_status_flag = default_cfg_status_flag;

	cfg_edit_mode = default_cfg_edit_mode;

	cfg_misc_flag = default_cfg_misc_flag;
	cfg_lapse_flag = default_cfg_lapse_flag;

	cfg_header_click_block_flag = default_cfg_header_click_block_flag;

#ifdef REC_AUDIO
	cfg_dst_rec_path = default_cfg_dst_rec_path;
#endif
	cfg_txt_filter = default_cfg_txt_filter;
	cfg_tf_filter = default_cfg_tf_filter;

	cfg_rq_wait = default_cfg_rq_wait;

	if(g_hWndCurrentTab == g_hWndTabDialog[CONF_0_TAB]) {
		init_config_0_dialog(g_hWndCurrentTab, false);
	}
	if (g_hWndCurrentTab == g_hWndTabDialog[CONF_1_TAB]) {
		init_config_1_dialog(g_hWndCurrentTab, false);
	}
	if (g_hWndCurrentTab == g_hWndTabDialog[CONF_2_TAB]) {
		init_config_2_dialog(g_hWndCurrentTab, false);
	}

	build_current_cfg(g_hWndCurrentTab);

	OnChanged();

	return FALSE;
}

LRESULT CBookmarkPreferences::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {

	return FALSE;
}

void CBookmarkPreferences::show_tab(unsigned int itab) {

	if (itab >= 0 && itab < NUM_TABS) {
		if (g_hWndCurrentTab != nullptr) {
			::ShowWindow(g_hWndCurrentTab, SW_HIDE);
		}

		g_current_tab = (t_uint32)::SendDlgItemMessage(m_hWnd, IDC_TAB_CFG, TCM_GETCURSEL, itab, 0);

		g_hWndCurrentTab = g_hWndTabDialog[itab];

		::ShowWindow(g_hWndCurrentTab, SW_SHOW);
	}
}

inline void set_window_text(HWND wnd, int IDC, const pfc::string8 &text) {
	pfc::stringcvt::string_wide_from_ansi wtext(text);
	::SetWindowText(::uGetDlgItem(wnd, IDC), (LPCTSTR)const_cast<wchar_t*>(wtext.get_ptr()));
}

inline void InitComboDate(HWND hwndParent, UINT idc_date, pfc::string8 strval) {

	std::vector<std::string> vfd = {
		"%a %d %b %H:%M:%S %Y",
		"%a %b %d %H:%M:%S %Y",
		"%d-%m-%y %H:%M:%S %a",
		"%y-%m-%d %H:%M:%S %a",
		"%d-%m-%y %H:%M:%S",
		"%y-%m-%d %H:%M:%S",
		"%d-%m-%y %H:%M %a",
		"%y-%m-%d %H:%M %a",
		"%d-%m-%y %H:%M",
		"%y-%m-%d %H:%M",
	};

	CComboBox cmb = GetDlgItem(hwndParent, idc_date);
	if (!cmb.GetCount()) {
		for (auto& w : vfd) {
			WCHAR wstr[DATE_BUFFER_SIZE];
			ConvertString8(w.c_str(), wstr, DATE_BUFFER_SIZE - 1);
			cmb.SetItemData(cmb.AddString(wstr), cmb.GetCount());
		}
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

void CBookmarkPreferences::init_config_0_dialog(HWND wnd, bool subclass) {

	InitComboDate(wnd, eat_date.idc, eat_date.cfg->get_value());

	cfgToUi(wnd, eat_format);
	cfgToUi(wnd, eat_date);
	cfgToUi(wnd, bab_display_ms);

	cfgToUi(wnd, eat_lapse);

	cfgToUi(wnd, bab_as_exit);
	cfgToUi(wnd, bab_as_newtrack);
	cfgToUi(wnd, bab_as_focus_newtrack);
	cfgToUi(wnd, bab_as_radio_newtrack);
	cfgToUi(wnd, bab_as_radio_comment);

	cfgToUi(wnd, bai_queue_flag, QUEUE_RESTORE_TO_FLAG, /*IDC_QUEUE_FLAG*/bai_queue_flag.idc);
	cfgToUi(wnd, bai_queue_flag, QUEUE_FLUSH_FLAG, IDC_QUEUE_FLUSH_FLAG);

	cfgToUi(wnd, bai_queue_flag, PLAY_ON_INIT_FLAG, IDC_PLAY_ON_INIT_FLAG);
	cfgToUi(wnd, bai_queue_flag, RQ_ON_INIT_FLAG, IDC_RQ_ON_INIT_FLAG);

	cfgToUi(wnd, bai_status_flag);

	cfgToUi(wnd, bab_edit_mode);

	cfgToUi(wnd, bai_misc_flag, MISC_FLAG_EDIT_ENTER_KEY_ADV, bai_misc_flag.idc);
	cfgToUi(wnd, bai_misc_flag, MISC_FLAG_EDIT_1CLK_EDIT, IDC_1CLK_EDIT_MODE);
	cfgToUi(wnd, bai_misc_flag, MISC_FLAG_INSTANT_WRITE_ON_EDITS, IDC_MISC_FLAG_WRITE_ON_EDITS);
	cfgToUi(wnd, bai_misc_flag, MISC_DUP_ENABLED_FLAG, IDC_MISC_FLAG_DUP_ENABLED);
	cfgToUi(wnd, bai_misc_flag, MISC_DUP_REMOVE_PREV_FLAG, IDC_MISC_FLAG_DUP_REMOVE_PREV);

	cfgToUi(wnd, bai_lapse_flag, LAPSE_FLAG_ENABLED, IDC_LAPSE_FLAG);

	cfgToUi(wnd, eat_header_click_block_flag);

	cfgToUi(wnd, eat_rq_wait);

	//dark mode
	m_dark.AddControls(wnd);
}

void CBookmarkPreferences::init_config_1_dialog(HWND wnd, bool subclass) {

	cfgToUi(wnd, eat_as_newtrack_playlists);
	cfgToUi(wnd, bab_as_filter_newtrack);

	cfgToUi(wnd, eat_txt_filter);
	cfgToUi(wnd, eat_tf_filter);

	cfgToUi(wnd, eat_rq_wait);
	//dark mode
	m_dark.AddControls(wnd);
}

void CBookmarkPreferences::init_config_2_dialog(HWND wnd, bool subclass) {

	pfc::string8 info =
		"About radio filters: \n\n" //
		"Primary radio field text filters apply only to title, artist and album when available.\n" //
		"The radio title format filter is active when returning a number > 0 or filtered text.\n\n" //
		"$if($cont_radio_filters(value),1,0) returns 1 when the value contains a text filter.\n\n" //
		"$if($in_radio_filters(value),1,0) returns 1 when the value matches a text filter.\n\n" //
		"%rdtd% is the current track descriptor value.\n\n" //
		"Disabling the callback monitor and verbose console logging may reduce workloads.\n" //
		"eg. receiving large and very frequent radio station metadata packets\n";

	uSetDlgItemText(wnd, IDC_STATIC_INFO, info);

	cfgToUi(wnd, bab_verbose);
	cfgToUi(wnd, bab_monitor);
}

bool CBookmarkPreferences::cfg_config_0_has_changed() {

	HWND wnd = g_hWndCurrentTab;

	bool result = false;
	result |= isUiChanged(wnd, eat_format);
	result |= isUiChanged(wnd, eat_date);
	result |= isUiChanged(wnd, bab_display_ms);

	result |= isUiChanged(wnd,eat_lapse);

	result |= isUiChanged(wnd,bab_as_exit);
	result |= isUiChanged(wnd,bab_as_newtrack);
	result |= isUiChanged(wnd,bab_as_focus_newtrack);
	result |= isUiChanged(wnd,bab_as_radio_newtrack);
	result |= isUiChanged(wnd,bab_as_radio_comment);

	int ui_fval = 0;
	ui_fval = ::IsDlgButtonChecked(wnd, /*IDC_QUEUE_FLAG*/bai_queue_flag.idc) ? QUEUE_RESTORE_TO_FLAG : ui_fval;
	ui_fval = ::IsDlgButtonChecked(wnd, IDC_QUEUE_FLUSH_FLAG) ? ui_fval | QUEUE_FLUSH_FLAG : ui_fval;
	ui_fval = ::IsDlgButtonChecked(wnd, IDC_PLAY_ON_INIT_FLAG) ? ui_fval | PLAY_ON_INIT_FLAG : ui_fval;
	ui_fval = ::IsDlgButtonChecked(wnd, IDC_RQ_ON_INIT_FLAG) ? ui_fval | RQ_ON_INIT_FLAG : ui_fval;

	result |= isUiChanged(wnd,bai_queue_flag, ui_fval);

	result |= isUiChanged(wnd,bai_status_flag);

	result |= isUiChanged(wnd,bab_edit_mode);

	ui_fval = 0;
	ui_fval = ::IsDlgButtonChecked(wnd, /*IDC_MISC_FLAG_ENTER_KEY_DOWN*/bai_misc_flag.idc) ? MISC_FLAG_EDIT_ENTER_KEY_ADV : ui_fval;
	ui_fval = ::IsDlgButtonChecked(wnd, IDC_1CLK_EDIT_MODE) ? ui_fval | MISC_FLAG_EDIT_1CLK_EDIT : ui_fval;
	ui_fval = ::IsDlgButtonChecked(wnd, IDC_MISC_FLAG_WRITE_ON_EDITS) ? ui_fval | MISC_FLAG_INSTANT_WRITE_ON_EDITS : ui_fval;
	ui_fval = ::IsDlgButtonChecked(wnd, IDC_MISC_FLAG_DUP_ENABLED) ? ui_fval | MISC_DUP_ENABLED_FLAG : ui_fval;
	ui_fval = ::IsDlgButtonChecked(wnd, IDC_MISC_FLAG_DUP_REMOVE_PREV) ? ui_fval | MISC_DUP_REMOVE_PREV_FLAG : ui_fval;

	result |= isUiChanged(wnd,bai_misc_flag, ui_fval);

	ui_fval = 0;
	ui_fval = ::IsDlgButtonChecked(wnd, IDC_LAPSE_FLAG) ? ui_fval | LAPSE_FLAG_ENABLED : ui_fval;

	result |= isUiChanged(wnd,bai_lapse_flag, ui_fval);

	result |= isUiChanged(wnd,eat_header_click_block_flag);

	result |= isUiChanged(wnd,eat_rq_wait);
	return result;
}


bool CBookmarkPreferences::cfg_config_1_has_changed() {

	HWND wnd = g_hWndCurrentTab;

	bool result = false;

	result |= isUiChanged(wnd, bab_as_filter_newtrack);
	result |= isUiChanged(wnd, eat_as_newtrack_playlists);

	result |= isUiChanged(wnd, eat_txt_filter);
	result |= isUiChanged(wnd, eat_tf_filter);

	return result;
}

bool CBookmarkPreferences::cfg_config_2_has_changed() {

	HWND wnd = g_hWndCurrentTab;

	bool result = false;
	result |= isUiChanged(wnd,bab_verbose);
	result |= isUiChanged(wnd,bab_monitor);
	return result;
}

void CBookmarkPreferences::save_config_0_dialog(HWND wnd, bool refresh) {

	bool bneedReload = false;

	uiToCfg(wnd, eat_format);
	uiToCfg(wnd, eat_date);
	uiToCfg(wnd, bab_display_ms);

	pfc::string8 buffer;
	buffer = uGetDlgItemText(wnd, eat_lapse.idc);
	if (atoi(buffer) >= 1 && atoi(buffer) <= 60) {
		uiToCfg(wnd, eat_lapse);
	}
	else {
		cfgToUi(wnd, eat_lapse);
	}

	buffer = uGetDlgItemText(wnd, eat_rq_wait.idc);
	if (atoi(buffer) >= 5 && atoi(buffer) <= 60) {
		uiToCfg(wnd, eat_rq_wait);
	}
	else {
		cfgToUi(wnd, eat_rq_wait);
	}

	uiToCfg(wnd, bab_as_exit);
	uiToCfg(wnd, bab_as_newtrack);
	uiToCfg(wnd, bab_as_focus_newtrack);
	uiToCfg(wnd, bab_as_radio_newtrack);

	if (bab_as_newtrack.cfg->get()) {
		g_bmAuto.updateDummy();
	}

	int ui_fval = 0;
	ui_fval = ::IsDlgButtonChecked(wnd, /*IDC_QUEUE_FLAG*/bai_queue_flag.idc) ? QUEUE_RESTORE_TO_FLAG : ui_fval;
	ui_fval = ::IsDlgButtonChecked(wnd, IDC_QUEUE_FLUSH_FLAG) ? ui_fval | QUEUE_FLUSH_FLAG : ui_fval;
	ui_fval = ::IsDlgButtonChecked(wnd, IDC_PLAY_ON_INIT_FLAG) ? ui_fval | PLAY_ON_INIT_FLAG : ui_fval;
	ui_fval = ::IsDlgButtonChecked(wnd, IDC_RQ_ON_INIT_FLAG) ? ui_fval | RQ_ON_INIT_FLAG : ui_fval;
	uiToCfg(wnd, bai_queue_flag, ui_fval);

	uiToCfg(wnd, bai_status_flag);
	uiToCfg(wnd, bab_edit_mode);

	ui_fval = 0;
	ui_fval = ::IsDlgButtonChecked(wnd, /*IDC_MISC_FLAG_ENTER_KEY_DOWN*/bai_misc_flag.idc) ? MISC_FLAG_EDIT_ENTER_KEY_ADV : ui_fval;
	ui_fval = ::IsDlgButtonChecked(wnd, IDC_1CLK_EDIT_MODE) ? ui_fval | MISC_FLAG_EDIT_1CLK_EDIT : ui_fval;
	ui_fval = ::IsDlgButtonChecked(wnd, IDC_MISC_FLAG_WRITE_ON_EDITS) ? ui_fval | MISC_FLAG_INSTANT_WRITE_ON_EDITS : ui_fval;
	ui_fval = ::IsDlgButtonChecked(wnd, IDC_MISC_FLAG_DUP_ENABLED) ? ui_fval | MISC_DUP_ENABLED_FLAG : ui_fval;
	ui_fval = ::IsDlgButtonChecked(wnd, IDC_MISC_FLAG_DUP_REMOVE_PREV) ? ui_fval | MISC_DUP_REMOVE_PREV_FLAG : ui_fval;
	uiToCfg(wnd, bai_misc_flag, ui_fval);

	ui_fval = 0;
	ui_fval = ::IsDlgButtonChecked(wnd, IDC_LAPSE_FLAG) ? ui_fval | LAPSE_FLAG_ENABLED : ui_fval;
	uiToCfg(wnd, bai_lapse_flag, ui_fval);

	uiToCfg(wnd, eat_header_click_block_flag);

	if (refresh) {
		for (auto gui : g_guiLists) {
			gui->ReloadItems(bit_array_true());
		}
	}

	OnChanged();
}


void CBookmarkPreferences::save_config_1_dialog(HWND wnd, bool dlgbind) {
	bool bres = false;
	bool bneedReload = false;

	uiToCfg(g_hWndCurrentTab, eat_as_newtrack_playlists);
	uiToCfg(wnd, bab_as_filter_newtrack);

	if (bab_as_newtrack.cfg->get()) {
		g_bmAuto.updateDummy();
	}

	uiToCfg(wnd, eat_txt_filter);
	uiToCfg(wnd, eat_tf_filter);

	if (bneedReload) {
		for (auto gui : g_guiLists) {
			gui->ReloadItems(bit_array_true());
		}
	}
}

void CBookmarkPreferences::save_config_2_dialog(HWND wnd, bool dlgbind) {

	uiToCfg(wnd, bab_verbose);
	uiToCfg(wnd, bab_monitor);

}

void CBookmarkPreferences::on_add_active_playlist() {

	size_t act_index = playlist_manager::get()->get_active_playlist();
	if (act_index == SIZE_MAX) { return; }

	pfc::string newName;
	playlist_manager::get()->playlist_get_name(act_index, newName);

	//replace all commas with dots (because of the comma-seperated list)
	newName.replace_char(',', '.');

	pfc::string8 curr_filter;
	uGetDlgItemText(g_hWndCurrentTab, IDC_AUTOSAVE_TRACK_FILTER, curr_filter);
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

	wchar_t fieldContent[1 + (kStringLength * 2)];
	::GetDlgItemTextW(g_hWndCurrentTab, IDC_AUTOSAVE_TRACK_FILTER, (LPTSTR)fieldContent, kStringLength);

	if (fieldContent[0] != L"\0"[0]) {
		wcscat_s(fieldContent, L",");
	}

	WCHAR wstr[1024];
	ConvertString8(newName, wstr, 1024 - 1);

	wcscat_s(fieldContent, wstr);

	::SetDlgItemText(g_hWndCurrentTab, IDC_AUTOSAVE_TRACK_FILTER, fieldContent);

}

void CBookmarkPreferences::on_menu_header_click_block() {

	CRect rcButton;
	HWND hwndCtrl = ::GetDlgItem(g_hWndCurrentTab, IDC_BUTTON_HEADER_CB);
	::GetWindowRect(hwndCtrl, rcButton);

	POINT pt = {};
	pt.x = rcButton.left;
	pt.y = rcButton.bottom;

	pfc::string8 currStrFlag = uGetDlgItemText(g_hWndCurrentTab, IDC_HIDDEN_HEADER_CLICK_BLOCK_FLAG);
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

	int cmd = TrackPopupMenu(hSplitMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, pt.x, pt.y, 0, g_hWndCurrentTab, NULL);
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

	uSetDlgItemText(g_hWndCurrentTab, IDC_HIDDEN_HEADER_CLICK_BLOCK_FLAG, std::to_string(tmpFlag).c_str());
	OnChanged();

}


INT_PTR WINAPI CBookmarkPreferences::on_config_0_dialog_message(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_INITDIALOG:
		setting_dlg = true;
		init_config_0_dialog(wnd, true);
		setting_dlg = false;
		return TRUE;
	case WM_COMMAND:
		if (HIWORD(wp) == CBN_SELCHANGE) {
			OnChanged();
		}
		else if (LOWORD(wp) == IDC_CMB_DATEFORMAT && HIWORD(wp) == CBN_SELCHANGE) {

			if (LOWORD(wp) == IDC_CMB_DATEFORMAT) {
				bool bComboDateChanged = (LOWORD(wp) == IDC_CMB_DATEFORMAT && HIWORD(wp) == CBN_SELCHANGE);
				if (bComboDateChanged) {
					OnComboChange(0, IDC_CMB_DATEFORMAT, g_hWndCurrentTab);
				}
				OnChanged();
			}
		}
		else if ((HIWORD(wp) == BN_CLICKED) || (HIWORD(wp) == EN_UPDATE)) {

				UINT nId = LOWORD(wp);
				if (nId == IDC_PREVIEW) {
					return FALSE;
				}
				if (nId == IDC_BUTTON_HEADER_CB) {
					on_menu_header_click_block();
					OnChanged();
					return FALSE;
				}
				if (nId == IDC_PLAY_ON_INIT_FLAG) {

					CWindow(::GetDlgItem(wnd, IDC_RQ_ON_INIT_FLAG)).EnableWindow((bool)((::IsDlgButtonChecked(wnd, IDC_PLAY_ON_INIT_FLAG) & BST_CHECKED) &&
						(!::IsDlgButtonChecked(wnd, IDC_QUEUE_FLAG) & BST_CHECKED)));
					OnChanged();
					return FALSE;
				}
				else if (nId == IDC_QUEUE_FLAG) {

					CWindow(::GetDlgItem(wnd, IDC_RQ_ON_INIT_FLAG)).EnableWindow(!::IsDlgButtonChecked(wnd, IDC_QUEUE_FLAG) & BST_CHECKED);
					OnChanged();
					return FALSE;
				}
				else if (nId == IDC_MISC_FLAG_DUP_ENABLED) {

					CWindow(::GetDlgItem(wnd, IDC_MISC_FLAG_DUP_REMOVE_PREV)).EnableWindow(::IsDlgButtonChecked(wnd, IDC_MISC_FLAG_DUP_ENABLED) & BST_CHECKED);
					OnChanged();
					return FALSE;
				}
				else if (nId == IDC_AUTOSAVE_RADIO_TRACK) {

					CWindow(::GetDlgItem(wnd, IDC_AUTOSAVE_RADIO_COMMENT_ST)).EnableWindow(::IsDlgButtonChecked(wnd, IDC_AUTOSAVE_RADIO_TRACK) & BST_CHECKED);
					OnChanged();
					return FALSE;
				}
				if (!setting_dlg && HIWORD(wp) == EN_UPDATE) {
					OnChanged();
					return FALSE;
				}
				else {
					if (HIWORD(wp) == BN_CLICKED) {
						OnChanged();
						return FALSE;
					}
				}
			}
		}
		return FALSE;
}

INT_PTR WINAPI CBookmarkPreferences::config_0_dialog_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	CBookmarkPreferences * p_this = nullptr;

	if (msg == WM_INITDIALOG) {
		p_this = (CBookmarkPreferences*)(lParam);
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LPARAM)p_this);
	}
	else {
		p_this = reinterpret_cast<CBookmarkPreferences*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}
	return p_this ? p_this->on_config_0_dialog_message(hWnd, msg, wParam, lParam) : FALSE;
}

INT_PTR WINAPI CBookmarkPreferences::on_config_1_dialog_message(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_INITDIALOG:
		setting_dlg = true;
		init_config_1_dialog(wnd, true);
		setting_dlg = false;
		return TRUE;
	case WM_COMMAND:

		if (HIWORD(wp) == CBN_SELCHANGE) {
			OnChanged();
			return FALSE;
		}
		else if ((HIWORD(wp) == BN_CLICKED) || (HIWORD(wp) == EN_UPDATE)) {

			UINT nId = LOWORD(wp);
			if (nId == IDC_PREVIEW) {
				return FALSE;
			}
			if (nId == IDC_BUTTON_AUTO_ADD_ACTIVE_PLAYLIST) {
				on_add_active_playlist();
				OnChanged();
				return FALSE;
			}
			return FALSE;
		}
	}
	return FALSE;
}

INT_PTR WINAPI CBookmarkPreferences::config_1_dialog_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	CBookmarkPreferences* p_this;
	if (msg == WM_INITDIALOG) {
		p_this = (CBookmarkPreferences*)(lParam);
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LPARAM)p_this);
	}
	else {

		p_this = reinterpret_cast<CBookmarkPreferences*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}
	return p_this ? p_this->on_config_1_dialog_message(hWnd, msg, wParam, lParam) : FALSE;
}

INT_PTR WINAPI CBookmarkPreferences::config_2_dialog_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	CBookmarkPreferences* p_this = nullptr;
	if (msg == WM_INITDIALOG) {
		p_this = (CBookmarkPreferences*)(lParam);
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LPARAM)p_this);
	}	else {
		p_this = reinterpret_cast<CBookmarkPreferences*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}
	return p_this ? p_this->on_config_2_dialog_message(hWnd, msg, wParam, lParam) : FALSE;
}

INT_PTR WINAPI CBookmarkPreferences::on_config_2_dialog_message(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_INITDIALOG:
		setting_dlg = true;
		init_config_2_dialog(wnd, true);
		setting_dlg = false;
		return TRUE;
	case WM_COMMAND:

		if (HIWORD(wp) == CBN_SELCHANGE) {
			OnChanged();
			return FALSE;
		}
		else if ((HIWORD(wp) == BN_CLICKED) || (HIWORD(wp) == EN_UPDATE)) {

			UINT nId = LOWORD(wp);
			if (nId == IDC_PREVIEW) {
				return FALSE;
			}
			if (!setting_dlg && HIWORD(wp) == EN_UPDATE) {
				OnChanged();
				return FALSE;
			}
			else {
				if (HIWORD(wp) == BN_CLICKED) {
					OnChanged();
					return FALSE;
				}
			}
			return FALSE;
		}
	}
	return FALSE;
}

t_uint32 CBookmarkPreferences::get_state() {

	t_uint32 state = preferences_state::resettable;
	if (HasChanged()) state |= preferences_state::changed;
	return state | preferences_state::dark_mode_supported;
}

void CBookmarkPreferences::apply() {

	bool bneedReload = g_current_tab == CONF_0_TAB && (isUiChanged(g_hWndCurrentTab, bab_display_ms) ||
			isUiChanged(g_hWndCurrentTab, eat_date));

	if (g_hWndCurrentTab == g_hWndTabDialog[CONF_0_TAB]) {
		save_config_0_dialog(g_hWndCurrentTab, false);
	}
	else if (g_hWndCurrentTab == g_hWndTabDialog[CONF_1_TAB]) {
		save_config_1_dialog(g_hWndCurrentTab, false);
	}

	if (bneedReload) {
		for (auto gui : g_guiLists) {
			gui->ReloadItems(bit_array_true());
		}
	}

	OnChanged();
}

bool CBookmarkPreferences::HasChanged() {

	bool bchanged = false;

	if (g_hWndCurrentTab == g_hWndTabDialog[CONF_0_TAB]) {
		bchanged = cfg_config_0_has_changed();
	}
	else if (g_hWndCurrentTab == g_hWndTabDialog[CONF_1_TAB]) {
		bchanged = cfg_config_1_has_changed();
	}
	else if (g_hWndCurrentTab == g_hWndTabDialog[CONF_2_TAB]) {
		bchanged = cfg_config_2_has_changed();
	}
	return bchanged;
}

void CBookmarkPreferences::OnChanged() {

	bool changed_desc_tf = g_current_tab == 0 && (isUiChanged(g_hWndCurrentTab, eat_format) || isUiChanged(g_hWndCurrentTab, eat_date));
	changed_desc_tf |= g_current_tab == 1 && (isUiChanged(g_hWndCurrentTab, eat_txt_filter) || isUiChanged(g_hWndCurrentTab, eat_tf_filter));
	ectrlAndString_t eat_txt_filter = { IDC_EDIT_AUTO_TXT_FILTER, &cfg_txt_filter, default_cfg_txt_filter };

	if (changed_desc_tf) {
		RefreshTitleFormatResults();
	}

	//enable/disable the apply button
	m_callback->on_state_changed();
}

void CBookmarkPreferences::RefreshTitleFormatResults() {

	pfc::string8 titleformat;
	pfc::string8 radio_filters;
	pfc::string8 tf_filter;

	if (g_current_tab == CONF_0_TAB) {
		titleformat = uGetDlgItemText(g_hWndCurrentTab, IDC_TITLEFORMAT);
		radio_filters = cfg_tf_filter;
		tf_filter = cfg_txt_filter.get_value();
	}
	else {
		titleformat = cfg_desc_format.get_value();
		radio_filters = uGetDlgItemText(g_hWndCurrentTab, IDC_EDIT_AUTO_TXT_FILTER);
		tf_filter = uGetDlgItemText(g_hWndCurrentTab, IDC_EDIT_AUTO_TF_FILTER);
	}

	titleformat_object::ptr p_script;
	static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(p_script, titleformat);

	pfc::string_formatter songDesc;
	bookmark_t bm = g_bmAuto.getDummy();

	bool playback_ok = false;

	if (bm.isRadio()) {

		bool test_playback_ok = true;
		pfc::string8 test_songDesc = bm.get_fdn();

		if (!test_songDesc.get_length()) {

			titleformat_object::ptr p_test_script;
			static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(p_script, "%title%");
			test_playback_ok = m_playback_control->playback_format_title(NULL, test_songDesc, p_script, NULL, playback_control::display_level_all);
		}

		filters::radio_nfo_type rnt;
		filters::get_radio_nfo(test_songDesc, rnt);

		radio_filter_titleformat_hook ra_hook;
		std::vector<pfc::string8>vfilters;

		filters::get_filters(radio_filters, vfilters);
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
	::SetDlgItemTextW(g_hWndCurrentTab, IDC_PREVIEW, os_tag_name);
	OnComboChange(0, IDC_CMB_DATEFORMAT, NULL);

}

class preferences_page_myimpl : public preferences_page_impl<CBookmarkPreferences> {

public:

	const char* get_name() { return COMPONENT_NAME_HC; }
	GUID get_guid() { return guid_bookmark_pref_page; }
	GUID get_parent_guid() { return guid_tools; }
};

static preferences_page_factory_t<preferences_page_myimpl> g_preferences_page_myimpl_factory;