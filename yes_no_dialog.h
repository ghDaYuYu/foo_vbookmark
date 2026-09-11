#pragma once

class CYesNoApiDialog {

public:

	void cancel() {
		m_query.reply.detach();
		m_query.reply = nullptr;
		popup_message_v3::get().release();
	}

		int query(HWND wndParent, std::vector<pfc::string8> values, bool bcancel = false, bool bwarning = true) {

			int res = ~0;

			completion_notify::ptr reply;
			fb2k::completionNotifyFunc_t comp_func = [&res](unsigned op) {
				res = op;
			};

			completion_notify::ptr comp_notify = fb2k::makeCompletionNotify(comp_func);

			m_query = { 0 };
			m_query.wndParent = wndParent;
			m_query.title = values[0];
			m_query.msg = values[1];
			m_query.icon = bwarning ? popup_message_v3::iconWarning : popup_message_v3::iconInformation;
			m_query.buttons = popup_message_v3::buttonYes | popup_message_v3::buttonNo;
			if (bcancel) {
				m_query.buttons |= popup_message_v3::buttonCancel;
			}
			m_query.reply = comp_notify;

			popup_message_v3::get()->show_query_modal(m_query);

		//owner task may be gone by now
			if (!bcancel) m_query = { 0 };

			if (res == popup_message_v3::buttonYes) {
				return 1;
			}
			else if (bcancel) {
				return (res == popup_message_v3::buttonNo ? 2 : 0);
			}
			else {
				return 0;
			}
		}

		HWND GetParentWindow() { return m_query.wndParent; }

private:
	popup_message_v3::query_t m_query = { 0 };
	};

