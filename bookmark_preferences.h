#pragma once

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

#define QUEUE_RESTORE_TO_FLAG                 1 << 0
#define QUEUE_FLUSH_FLAG                      1 << 1
#define QUEUE_CUST2_FLAG                      1 << 2
//cfg_status_flag
#define STATUS_PAUSED_FLAG                    1 << 0
#define STATUS_CUST_FLAG                      1 << 1
#define STATUS_CUST2_FLAG                     1 << 2
//cfg_misc_flag
#define MISC_FLAG_EDIT_ENTER_KEY_ADV          1 << 0
#define MISC_FLAG_INSTANT_WRITE_ON_EDITS      1 << 1
#define MISC_CUST3_FLAG                       1 << 2

#define LAPSE_FLAG_ENABLED                    1 << 0

  inline bool is_cfg_Bookmarking() { return !(cfg_status_flag.get_value() & STATUS_PAUSED_FLAG); }
  inline bool is_cfg_Queuing() { return cfg_queue_flag.get_value() & QUEUE_RESTORE_TO_FLAG; }
  inline bool is_cfg_Flush_Queue() { return cfg_queue_flag.get_value() & QUEUE_FLUSH_FLAG; }
  inline bool is_cfg_Enter_Key_Adv() { return cfg_misc_flag.get_value() & MISC_FLAG_EDIT_ENTER_KEY_ADV; }
  inline bool is_cfg_Instant_Write() { return cfg_misc_flag.get_value() & MISC_FLAG_INSTANT_WRITE_ON_EDITS; }
  inline bool is_cfg_LapseEnabled() { return cfg_lapse_flag.get_value() & LAPSE_FLAG_ENABLED; }
  inline int get_cfg_lapse() { return atoi(cfg_lapse.get_value()); }
  extern GUID g_get_prefs_guid();