#pragma once

#include <guiddef.h>

#include "pfc/int_types.h"
#include "pfc/string-lite.h"

#define DATE_BUFFER_SIZE 255
#define LOC_RETRIES 3

inline static const int kUI_CONF_VER = 2;
inline constexpr double KMin_Lapse = 2.0;

struct bookmark_t {

private:
	double time = 0.0;
	pfc::string8 name;
	pfc::string8 desc;
	pfc::string8 comment;
	pfc::string8 fdn;
	bool dyna = false;

public:

	bit_array_bittable sel_mask;

	GUID guid_bm = pfc::guid_null;
	pfc::string8 path;
	pfc::string8 date;
	pfc::string8 runtime_date;
	t_uint32 subsong = 0;

	bool need_playlist = true;
	t_uint32 need_loc_retries = 0;

	pfc::string8 playlist;
	GUID guid_playlist = pfc::guid_null;

	void set_rt_time(double t) {
		t = (std::max)(0.0, t);
		time = t < KMin_Lapse ? 0.0 : t;
	}
	void set_time(double t, bool exact_time = false)) {
		if (exact_time) {
			set_exact_time(t);
			return;
		}
		t = (std::max)(0.0, t);
		time = t < KMin_Lapse ? 0.0 : t;
	}

	void set_exact_time(double t) {
		t = (std::max)(0.0, t);
		time = t;
	}

	void set_name(pfc::string8 p_name);
	void set_desc(pfc::string8 p_desc);
	void set_comment(pfc::string8 p_comment);
	void assign_desc(pfc::string8 p_adesc);
	void set_fdn(pfc::string8 p_fdn);

	void set_dyna(bool p_dyna) { dyna = p_dyna; };

	const pfc::string8 get_name(bool or_desc) const {
		return name.get_length() ? name : or_desc ? desc : "";
	}

	const double get_time() const { return time; }
	const pfc::string8 get_desc() const { return desc; }
	const pfc::string8 get_comment() const { return comment; }
	const pfc::string8 get_fdn() const { return fdn; }

	const bool get_dyna() const { return dyna; }

	bool isRadio() const {
		//todo
		return isRadio(path);
	}

	bool isRadio(const pfc::string8 path) const {
		return (path.startsWith("https://") || path.startsWith("http://")) && !path.contains("youtube") && !path.contains("youtu.be");
	}

	void reset() {
		*this = bookmark_t();
		this->guid_bm = pfc::createGUID();
	}

	void swap(bookmark_t& other)
	{
		bookmark_t tmp = other;

		other.guid_bm = this->guid_bm;
		other.comment.move(this->comment);
		other.playlist.move(this->playlist);
		other.guid_playlist = this->guid_playlist;
		other.date.move(this->date);
		other.runtime_date.move(this->runtime_date);
		other.name.move(this->name);
		other.fdn.move(this->fdn);
		other.desc.move(this->desc);
		other.path.move(this->path);
		other.subsong = this->subsong;
		other.time = this->time;

		guid_bm = tmp.guid_bm;
		comment.move(tmp.comment);
		playlist.move(tmp.playlist);
		guid_playlist = tmp.guid_playlist;
		date.move(tmp.date);
		runtime_date.move(tmp.runtime_date);
		name.move(tmp.name);
		fdn.move(tmp.fdn);
		desc.move(tmp.desc);
		path.move(tmp.path);
		subsong = tmp.subsong;
		time = tmp.time;
	}

	void resetDummyKeepDyna() {
		auto tmp = *this;
		reset();
		dyna = tmp.dyna;
		desc = tmp.desc;
		comment = tmp.comment;
	}

	inline void set_current_date() {

		pfc::string8 tmp_date;

		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);
		auto sctime = asctime(&tm);

		tmp_date.set_string(sctime);
		tmp_date.truncate_last_char();

		char buffer[DATE_BUFFER_SIZE];
		std::strftime(buffer, DATE_BUFFER_SIZE, cfg_date_format.get(), &tm);
		runtime_date = buffer;
		date.move(tmp_date);
	}

};

extern void unix_str_date_to_time(pfc::string8 unix_date, time_t& out_rawtime, tm& out_tm);

extern int get_month_index(std::string name);
extern int get_wday_index(std::string name);
