#pragma once
#include "helpers/win32_dialog.h"
#include "error_manager.h"

class tab_entry
{
public:
	LPSTR m_pszName;
	DLGPROC m_lpDialogFunc;
	WORD m_nID;

	tab_entry(LPSTR pszName, DLGPROC lpDialogFunc, WORD nID) {
		m_pszName = pszName;
		m_lpDialogFunc = lpDialogFunc;
		m_nID = nID;
	}
	tab_entry() {
		m_pszName = "";
		m_lpDialogFunc = nullptr;
		m_nID = 0;
	}

	HWND CreateTabDialog(HWND hWndParent, LPARAM lInitParam = 0) {
		PFC_ASSERT(m_lpDialogFunc != nullptr);
		HWND dlg = uCreateDialog(m_nID, hWndParent, m_lpDialogFunc, lInitParam);
		return dlg;
	}
};

template<class T>
class MyCDialogImpl : public CDialogImpl<T>, public ErrorManager
{
public:
	virtual void show() {
		ShowWindow(SW_SHOW);
	}

	virtual void hide() {
		ShowWindow(SW_HIDE);
	}

	virtual void destroy() {
		if (::IsWindow(m_hWnd)) {
			DestroyWindow();
		}
		else {
			delete this;
		}
	}
	virtual void enable(bool v) = 0;
};


#define MY_BEGIN_MSG_MAP(theClass) \
public: \
	BOOL ProcessWindowMessage(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, \
	_In_ LPARAM lParam, _Inout_ LRESULT& lResult, _In_ DWORD dwMsgMapID = 0) \
	{ \
	try { \
		BOOL bHandled = TRUE; \
		(hWnd); \
		(uMsg); \
		(wParam); \
		(lParam); \
		(lResult); \
		(bHandled); \
		switch (dwMsgMapID) \
			{ \
			case 0:

#define MY_END_MSG_MAP() \
	break; \
		default: \
		ATLTRACE(ATL::atlTraceWindowing, 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID); \
		ATLASSERT(FALSE); \
		break; \
		} \
		return FALSE; \
	} \
	catch (foo_vbookmarks_exception &e) { \
		add_error(e, true); \
		display_errors(); \
		clear_errors(); \
		return FALSE; \
	} \
	}
