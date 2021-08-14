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

	static const float _margin;
	static const float _icon_size;
	static const float _info_size;

#ifdef _WIN64
	unsigned long long gdi_plus_token_;
#else
	unsigned long gdi_plus_token_;
#endif

	bool _setting_darktheme = false;

	lecui::controls _ctrls{ *this };
	lecui::page_manager _page_man{ *this };
	lecui::appearance _apprnc{ *this };
	lecui::dimensions _dim{ *this };
	lecui::instance_manager _instance_man{ *this, _instance_guid };
	lecui::widget_manager _widget_man{ *this };
	lecui::timer_manager _timer_man{ *this };

	std::vector<image_info> _pictures;
	image_info _displayed_image;

	bool on_initialize(std::string& error) override;
	bool on_layout(std::string& error) override;
	void on_start() override;
	void about();

public:
	main_form(const std::string& caption);
	~main_form();
};
