#pragma once

#include "exception.h"
//#include "utils.h"

inline pfc::string8 join(const pfc::array_t<pfc::string8>& in, const pfc::string8& join_field) {
	pfc::string8 out = "";
	const size_t count = in.get_count();
	for (size_t i = 0; i < count; i++) {
		out << in[i];
		if (i != count - 1) {
			out << join_field;
		}
	}
	return out;
}

class ErrorManager
{
public:
	virtual ~ErrorManager() {
	}

protected:
	bool fatal_error = false;
	pfc::array_t<pfc::string8> errors;

	void add_error(const char* msg, bool fatal = false) {
		fatal_error = fatal_error || fatal;
		pfc::string8 error;
		error << (fatal ? "(stopped) " : "(skipped) ") /*<< "Exception: "*/ /*<< ": "*/ << msg;
		errors.append_single(error);
	}

	void add_error(foo_vbookmarks_exception& e, bool fatal = false) {
		fatal_error = fatal_error || fatal;
		pfc::string8 error;
		error << (fatal ? "(stopped) " : "(skipped) ") /*<< "Exception: "*/ /*<< ": "*/ << e.what();
		errors.append_single(error);
	}

	void add_error(const char* msg, foo_vbookmarks_exception& e, bool fatal = false) {
		fatal_error = fatal_error || fatal;
		pfc::string8 error;
		error << (fatal ? "(stopped) " : "(skipped) ") /*<< "Exception [" << msg << "]: "*/ << "[" << msg << "]: " << e.what();
		errors.append_single(error);
	}

	// displays errors in popup
	void display_errors() {
		if (errors.get_count()) {
			errors.append_single(pfc::string8("\n[ESCAPE to close]"));
			popup_message::g_show(join(errors, "\n").get_ptr(), "Exception", popup_message::icon_error);
		}
	}

	void clear_errors() {
		errors.force_reset();
	}
};
