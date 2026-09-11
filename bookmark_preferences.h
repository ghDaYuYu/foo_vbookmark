#pragma once

#include "helpers/atl-misc.h"
#include "resource.h"

#include "libPPUI/CDialogResizeHelper.h"
#include "helpers/DarkMode.h"
#include "mycdialog.h"
#include "exception.h"
#include "yes_no_dialog.h"

#include "header_static.h"

extern UINT UMSG_NEW_TRACK;
extern UINT UMSG_PAUSED;

#define NUM_TABS 3

#define CONF_0_TAB               0
#define CONF_1_TAB               1
#define CONF_2_TAB               2

//cfg_queue_flag
#define QUEUE_RESTORE_TO_FLAG                 1 << 0
#define QUEUE_FLUSH_FLAG                      1 << 1
#define QUEUE_CUST2_FLAG                      1 << 2
#define QUEUE_CUST3_FLAG                      1 << 3
#define QUEUE_CUST4_FLAG                      1 << 4
#define PLAY_ON_INIT_FLAG                     1 << 5
#define RQ_ON_INIT_FLAG                       1 << 6
//cfg_status_flag
#define STATUS_PAUSED_FLAG                    1 << 0
#define STATUS_CUST_FLAG                      1 << 1
#define STATUS_CUST2_FLAG                     1 << 2
//cfg_misc_flag
#define MISC_FLAG_EDIT_ENTER_KEY_ADV          1 << 0
#define MISC_FLAG_INSTANT_WRITE_ON_EDITS      1 << 1
#define MISC_FLAG_EDIT_1CLK_EDIT              1 << 2
#define MISC_DUP_ENABLED_FLAG                 1 << 3
#define MISC_DUP_REMOVE_PREV_FLAG             1 << 4
//cfg_lapse_flag
#define LAPSE_FLAG_ENABLED                    1 << 0

extern cfg_string cfg_desc_format;
extern cfg_string cfg_date_format;
extern cfg_bool cfg_display_ms;
extern cfg_string cfg_autosave_newtrack_playlists;

extern cfg_bool cfg_autosave_newtrack;
extern cfg_bool cfg_autosave_focus_newtrack;
extern cfg_bool cfg_autosave_radio_newtrack;
extern cfg_bool cfg_autosave_radio_comment;
extern cfg_bool cfg_autosave_filter_newtrack;
extern cfg_bool cfg_autosave_on_quit;

extern cfg_bool cfg_verbose;
extern cfg_bool cfg_monitor;

extern cfg_int cfg_queue_flag;
extern cfg_int cfg_status_flag;

extern cfg_bool cfg_edit_mode;

extern cfg_int cfg_misc_flag;

extern cfg_string cfg_lapse;
extern cfg_int cfg_lapse_flag;

extern cfg_string cfg_header_click_block_flag;
extern cfg_string cfg_txt_filter;
extern cfg_string cfg_tf_filter;

extern cfg_string cfg_rq_wait;

extern cfg_int cfg_last_tab;

inline bool is_cfg_Bookmarking() { return !(cfg_status_flag.get_value() & STATUS_PAUSED_FLAG); }

inline bool is_cfg_Queuing() { return cfg_queue_flag.get_value() & QUEUE_RESTORE_TO_FLAG; }
inline bool is_cfg_Flush_Queue() { return cfg_queue_flag.get_value() & QUEUE_FLUSH_FLAG; }
inline bool is_cfg_Play_OnInit() { return cfg_queue_flag.get_value() & PLAY_ON_INIT_FLAG; }
inline bool is_cfg_Rq_OnInit() { return cfg_queue_flag.get_value() & RQ_ON_INIT_FLAG; }

inline bool is_cfg_Enter_Key_Adv() { return cfg_misc_flag.get_value() & MISC_FLAG_EDIT_ENTER_KEY_ADV; }
inline bool is_cfg_1clk_Edit() { return !cfg_edit_mode.get() && cfg_misc_flag.get_value() & MISC_FLAG_EDIT_1CLK_EDIT; }
inline bool is_cfg_Instant_Write() { return cfg_misc_flag.get_value() & MISC_FLAG_INSTANT_WRITE_ON_EDITS; }
inline bool is_cfg_LapseEnabled() { return cfg_lapse_flag.get_value() & LAPSE_FLAG_ENABLED; }
inline bool is_cfg_Dupli_Enabled() { return cfg_misc_flag.get_value() & MISC_DUP_ENABLED_FLAG; }
inline bool is_cfg_Dupli_Remove_Prev() { return is_cfg_Dupli_Enabled() && cfg_misc_flag.get_value() & MISC_DUP_REMOVE_PREV_FLAG; }

inline int get_cfg_lapse() { return atoi(cfg_lapse.get_value()); }

inline int get_cfg_header_cb_flag() { return atoi(cfg_header_click_block_flag.get_value()); }
inline bool is_cfg_Header_Click_Blocked(size_t ndx) { return get_cfg_header_cb_flag() & (1 << ndx); }

//snapLeft, snapTop, snapRight, snapBottom
const CDialogResizeHelper::Param rz_params[] = {
	{IDC_TITLEFORMAT, 1,0,1,0},
};

static const pfc::string8 default_cfg_bookmark_desc_format = "%title% - $if2(%album% - ,)%artist%";
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

static const int default_cfg_misc_flag = 0/*MISC_FLAG_INSTANT_WRITE_ON_EDITS*/;

static const int default_cfg_lapse_flag = LAPSE_FLAG_ENABLED;

static const pfc::string8 default_cfg_header_click_block_flag = "0";

static const pfc::string8 default_cfg_rq_wait = "20";

#ifdef REC_AUDIO
static const pfc::string8 default_cfg_dst_rec_path = "";

#endif
static const pfc::string8 default_cfg_txt_filter = "Radio Classic Rock,RockClassics,Breaking News";
static const pfc::string8 default_cfg_tf_filter = "$if($or($strstr(%title%,ANEWSFM),$cont_radio_filters(%title% %artist%),$in_radio_filters(%title%)),1,0)";

static const int default_cfg_last_tab = 0;


extern HWND g_hWndCurrentTab;

class CBookmarkPreferences : public MyCDialogImpl<CBookmarkPreferences>, public CMessageFilter, public preferences_page_instance {

private:

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

	static_api_ptr_t<playback_control> m_playback_control;
	const preferences_page_callback::ptr m_callback;

	ectrlAndString_t eat_format = { IDC_TITLEFORMAT, &cfg_desc_format, default_cfg_bookmark_desc_format };
	ectrlAndString_t eat_date = { IDC_CMB_DATEFORMAT, &cfg_date_format, default_cfg_date_format };
	ectrlAndString_t eat_as_newtrack_playlists = { IDC_AUTOSAVE_TRACK_FILTER, &cfg_autosave_newtrack_playlists, default_cfg_autosave_newtrack_playlists };

	ectrlAndString_t eat_lapse = { IDC_LAPSE, &cfg_lapse, default_cfg_lapse };
	boxAndBool_t bab_display_ms = { IDC_DISPLAY_MS, &cfg_display_ms, default_cfg_display_ms };

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
#ifdef REC_AUDIO
	ectrlAndString_t eat_dst_rec_path = { IDC_EDIT_REC_DST, &cfg_dst_rec_path, default_cfg_dst_rec_path };
#endif
	ectrlAndString_t eat_txt_filter = { IDC_EDIT_AUTO_TXT_FILTER, &cfg_txt_filter, default_cfg_txt_filter };
	ectrlAndString_t eat_tf_filter = { IDC_EDIT_AUTO_TF_FILTER, &cfg_tf_filter, default_cfg_tf_filter };

	ectrlAndString_t eat_rq_wait = { IDC_RQ_WAIT, &cfg_rq_wait, default_cfg_rq_wait };

	void cfgToUi(HWND wnd, boxAndBool_t bab) {
		CCheckBox cb(::GetDlgItem(wnd, bab.idc));
		cb.SetCheck(bab.cfg->get());
	}

	void uiToCfg(HWND wnd, boxAndBool_t& bab) {
		CCheckBox cb(::GetDlgItem(wnd, bab.idc));
		bab.cfg->set((bool)cb.GetCheck());
	}

	void defToUi(HWND wnd, boxAndBool_t bab) {
		CCheckBox cb(::GetDlgItem(wnd, bab.idc));
		cb.SetCheck(bab.def);
	}

	bool isUiChanged(HWND wnd, boxAndBool_t bab) {
		CCheckBox cb(::GetDlgItem(wnd, bab.idc));
		return bab.cfg->get() != (bool)cb.GetCheck();
	}

	// boxAndInt_t

	void cfgToUi(HWND wnd, boxAndInt_t bai) {
		CCheckBox cb(::GetDlgItem(wnd, bai.idc));
		cb.SetCheck((bool)(bai.cfg->get_value()));
	}

	void cfgToUi(HWND wnd, boxAndInt_t bai, int flag, int idc) {
		CCheckBox cb(::GetDlgItem(wnd, idc));
		cb.SetCheck(bai.cfg->get_value() & flag);
	}

	void uiToCfg(HWND wnd, boxAndInt_t& bai) {
		CCheckBox cb(::GetDlgItem(wnd, bai.idc));
		bai.cfg->set(cb.GetCheck());
	}

	void uiToCfg(HWND wnd, boxAndInt_t& bai, int ui_fval) {
		bai.cfg->set(ui_fval);
	}

	void defToUi(HWND wnd, boxAndInt_t bai) {
		CCheckBox cb(::GetDlgItem(wnd, bai.idc));
		cb.SetCheck(bai.def);
	}

	void defToUi(HWND wnd, boxAndInt_t bai, int flag, int idc) {
		CCheckBox cb(::GetDlgItem(wnd, idc));
		cb.SetCheck(bai.def & flag);
	}

	bool isUiChanged(HWND wnd, boxAndInt_t bai) {
		CCheckBox cb(::GetDlgItem(wnd, bai.idc));
		return bai.cfg->get_value() != (int)cb.GetCheck();
	}

	bool isUiChanged(HWND wnd, boxAndInt_t bai, int ui_fval) {
		return bai.cfg->get_value() != ui_fval;
	}

	// ectrlAndString_t

	void cfgToUi(HWND wnd, ectrlAndString_t eat) {
		if (eat.idc == IDC_CMB_DATEFORMAT) {
			uSetDlgItemText(wnd, eat.idc, eat.cfg->get_value().c_str());
		}
		else {
			uSetDlgItemText(wnd, eat.idc, eat.cfg->get_value().c_str());
		}
	}

	void uiToCfg(HWND wnd, ectrlAndString_t& eat) {
		pfc::string8 buffer;
		if (eat.idc == IDC_CMB_DATEFORMAT) {
			buffer = uGetDlgItemText(wnd, eat.idc);
		}
		else {
			buffer = uGetDlgItemText(wnd, eat.idc);
		}
		eat.cfg->set(buffer.c_str());
	}

	void defToUi(HWND wnd, ectrlAndString_t eat) {
		if (eat.idc == IDC_CMB_DATEFORMAT) {
			uSetDlgItemText(wnd, eat.idc, eat.def.c_str());
		}
		else {
			uSetDlgItemText(wnd, eat.idc, eat.def.c_str());
		}
	}

	bool isUiChanged(HWND wnd, ectrlAndString_t eat) {
		pfc::string8 buffer;
		if (eat.idc == IDC_CMB_DATEFORMAT) {
			buffer = uGetDlgItemText(wnd, eat.idc);
		}
		else {
			buffer = uGetDlgItemText(wnd, eat.idc);
		}
		return !buffer.equals(eat.cfg->get_value());
	}

	static INT_PTR WINAPI config_0_dialog_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	INT_PTR WINAPI on_config_0_dialog_message(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static INT_PTR WINAPI config_1_dialog_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	INT_PTR WINAPI on_config_1_dialog_message(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static INT_PTR WINAPI config_2_dialog_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	INT_PTR WINAPI on_config_2_dialog_message(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void init_current_tab();

	void init_config_0_dialog(HWND wnd, bool subclass);
	void init_config_1_dialog(HWND wnd, bool subclass);
	void init_config_2_dialog(HWND wnd, bool subclass);

	bool build_current_cfg(bool reset);
	void pushcfg(bool reset);

	void save_config_0_dialog(HWND wnd, bool dlgbind);
	void save_config_1_dialog(HWND wnd, bool dlgbind);
	void save_config_2_dialog(HWND wnd, bool dlgbind);

	bool cfg_config_0_has_changed();
	bool cfg_config_1_has_changed();
	bool cfg_config_2_has_changed();

	bool HasChanged();
	void OnChanged();

	void on_menu_header_click_block();
	void on_add_active_playlist();

	void RefreshTitleFormatResults();

public:

	enum { IDD = IDD_DIALOG_CONF };

	t_uint32 get_state();
	void apply();
	void reset();

	virtual BOOL PreTranslateMessage(MSG* pMsg) override {
		return ::IsDialogMessage(m_hWnd, pMsg);
	}
	virtual void OnFinalMessage(HWND hWnd)
	{
		//...
	}

	BEGIN_MSG_MAP(CBookmarkPreferences)
	CHAIN_MSG_MAP_MEMBER(m_resize_helper)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_CODE_HANDLER_EX(CBN_SELCHANGE, OnComboChange)
	COMMAND_CODE_HANDLER_EX(EN_CHANGE, OnEditChange)
	NOTIFY_HANDLER(IDC_TAB_CFG, TCN_SELCHANGING, OnChangingTab)
	NOTIFY_HANDLER(IDC_TAB_CFG, TCN_SELCHANGE, OnChangeTab)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	MESSAGE_HANDLER_SIMPLE(UMSG_NEW_TRACK, OnNewTrackMessage)
	MESSAGE_HANDLER_SIMPLE(UMSG_PAUSED, OnPaused)
	END_MSG_MAP()

	void OnEditChange(UINT uNotifyCode, int nId, CWindow wndCtl);
	void OnComboChange(UINT uNotifyCode, int nId, CWindow wndCtl);
	LRESULT OnNewTrackMessage() { RefreshTitleFormatResults(); return 0; }
	LRESULT OnPaused() { cfgToUi(g_hWndCurrentTab, bai_status_flag); HasChanged(); return 0; }

	CBookmarkPreferences(preferences_page_callback::ptr callback) :	m_callback(callback), m_resize_helper(rz_params) {
		//..
	}

	~CBookmarkPreferences();

	void InitTabs();

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnChangingTab(WORD /*wNotifyCode*/, LPNMHDR /*lParam*/, BOOL& /*bHandled*/); 
	LRESULT OnChangeTab(WORD /*wNotifyCode*/, LPNMHDR /*lParam*/, BOOL& /*bHandled*/); 
	LRESULT OnDefaults(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	void show_tab(unsigned int itab);
	void enable(bool v) override {}

private:

	pfc::array_t<tab_entry> tab_table;

	bool setting_dlg = false;

	fb2k::CDarkModeHooks m_dark;
	CDialogResizeHelper m_resize_helper;
};

static const GUID guid_bookmark_pref_page = { 0x49e82acf, 0x4954, 0x4274, { 0x80, 0xe8, 0xff, 0x74, 0xf3, 0x71, 0x1e, 0x5f } };

inline  GUID g_get_prefs_guid() { return guid_bookmark_pref_page; }