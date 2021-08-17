/*
** MIT License
**
** Copyright(c) 2021 Alec Musasa
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this softwareand associated documentation files(the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions :
**
** The above copyright noticeand this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
*/

#pragma once

#include "version_info.h"
#include "resource.h"
#include "spotlight_images.h"

// lecui
#include <liblec/lecui/instance.h>
#include <liblec/lecui/controls.h>
#include <liblec/lecui/appearance.h>
#include <liblec/lecui/utilities/timer.h>
#include <liblec/lecui/utilities/tray_icon.h>
#include <liblec/lecui/widgets/widget.h>
#include <liblec/lecui/containers/page.h>

// leccore
#include <liblec/leccore/settings.h>
#include <liblec/leccore/web_update.h>

using namespace liblec;
using snap_type = lecui::rect::snap_type;

#ifdef _WIN64
#define architecture	"64bit"
#else
#define architecture	"32bit"
#endif

// the main form
class main_form : public lecui::form {
	const std::string _instance_guid = "{24DE7949-0EB7-4086-85F0-76D10191E633}";
	const std::string _install_guid_32 = "{3FA54748-169E-4B63-9871-1D3367545A9A}";
	const std::string _install_guid_64 = "{7BC77A87-2034-4483-94A8-2774D1A3C494}";
	const std::string _update_xml_url = "https://raw.githubusercontent.com/alecmus/spotlight_images/master/latest_update.xml";

	static const float _margin;
	static const float _icon_size;
	static const float _info_size;

#ifdef _WIN64
	unsigned long long gdi_plus_token_;
#else
	unsigned long gdi_plus_token_;
#endif

	lecui::controls _ctrls{ *this };
	lecui::page_manager _page_man{ *this };
	lecui::appearance _apprnc{ *this };
	lecui::dimensions _dim{ *this };
	lecui::instance_manager _instance_man{ *this, _instance_guid };
	lecui::widget_manager _widget_man{ *this };
	lecui::timer_manager _timer_man{ *this };

	std::vector<image_info> _pictures;
	image_info _displayed_image;

	bool _restart_now = false;

	// 1. If application is installed and running from an install directory this will be true.
	// 2. If application is installed and not running from an install directory this will also
	// be true unless there is a .portable file in the same directory.
	// 3. If application is not installed then portable mode will be used whether or not a .portable
	// file exists in the same directory.
	bool _installed;
	bool _real_portable_mode;
	bool _system_tray_mode;
	std::string _install_location_32, _install_location_64;
	leccore::settings& _settings;
	leccore::registry_settings _reg_settings{ leccore::registry::scope::current_user };
	leccore::ini_settings _ini_settings{ "spotlight_images.ini" };
	bool _setting_darktheme = false;
	bool _setting_autocheck_updates = true;
	leccore::check_update _check_update{ _update_xml_url };
	leccore::check_update::update_info _update_info;
	bool _setting_autodownload_updates = false;
	bool _update_check_initiated_manually = false;
	leccore::download_update _download_update;
	std::string _update_directory;
	bool _setting_autostart = false;
	std::string _folder;

	const bool _cleanup_mode;
	const bool _update_mode;
	const bool _recent_update_mode;

	lecui::tray_icon _tray_icon{ *this };

	bool _update_details_displayed = false;

	bool on_initialize(std::string& error) override;
	bool on_layout(std::string& error) override;
	void on_start() override;
	void on_close() override;
	void add_side_pane();
	void add_back_button();
	void add_home_page();
	void add_help_page();
	void add_settings_page();

	void updates();
	void on_update_check();
	void on_update_download();
	bool installed();
	void create_update_status();
	void close_update_status();
	void on_close_update_status();

	void on_darktheme(bool on);
	void on_autostart(bool on);
	void on_autocheck_updates(bool on);
	void on_autodownload_updates(bool on);
	void on_select_location();

public:
	main_form(const std::string& caption);
	~main_form();
	bool restart_now() {
		return _restart_now;
	}
};
