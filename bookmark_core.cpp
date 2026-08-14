#include "stdafx.h"

#include "bookmark_core.h"
#include "bookmark_list_dialog.h"

using namespace dlg;

namespace {


} //anonymous namespace

//==================Hooks for main menu=======================

void bbookmarkHook_store() { CListCtrlMarkDialog::addBookmark(); }
void bbookmarkHook_restore() { CListCtrlMarkDialog::restoreFocusedBookmark(); }
void bbookmarkHook_restoreActivePlaylist(size_t last_played) { CListCtrlMarkDialog::restoreBookmark(last_played, true); }
void bbookmarkHook_clear() { CListCtrlMarkDialog::clearBookmarks(); }

bool bbookmarkHook_canStore() { return CListCtrlMarkDialog::canStore(); }
bool bbookmarkHook_canRestore() { return CListCtrlMarkDialog::canRestore(); }
bool bbookmarkHook_canRestoreActivePlaylist(size_t & last_played) { return CListCtrlMarkDialog::canRestoreActivePlaylist(last_played); }
bool bbookmarkHook_canClear() { return CListCtrlMarkDialog::canClear(); }
