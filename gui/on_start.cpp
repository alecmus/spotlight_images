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

#include "../gui.h"
#include "../helper_functions.h"
#include <liblec/lecui/widgets/label.h>
#include <liblec/lecui/widgets/table_view.h>

#include <algorithm>

void main_form::on_start() {
	_pictures = fetch_images(_folder);

	// display caption
	std::string message = std::to_string(_pictures.size()) + " image";
	if (_pictures.size() != 1) message += "s";

	message += " copied.";

	if (_pictures.size() == 0) {
		message = "No images were copied.";

		_timer_man.add("no_images_timer", 100, [&]() {
			_timer_man.stop("no_images_timer");
			std::string display_text = "No images were found. Kindly check the following:\n\n"
				"1. Is Windows Spotlight enabled for the current user profile? Check under "
				"Settings - Personalization - Lock screen. After enabling Spotlight "
				"it may take up to 24 hours for the first image to show up.\n\n"
				"2. Is your internet connection set to metered? Check under "
				"Settings - Network and Internet - Properties. When the connection is metered Windows"
				" might not update the images in order to save data.";
			form::message(display_text);
			});
	}

	if (_pictures.size() != 0) {
		auto landscape = std::count_if(_pictures.begin(), _pictures.end(),
			[](image_info p) {
				return p.orientation == image_orientation::landscape;
			});

		message += " " + std::to_string(landscape) + " Landscape, " + std::to_string(_pictures.size() - landscape) + " Portrait.";
		message += " Select to preview.";
	}

	try {
		auto& caption = get_label("home/caption");
		caption.text(message);
	}
	catch (const std::exception&) {}

	// populate tableview
	try {
		auto& list = get_table_view("home/list");

		for (const auto& pic : _pictures) {
			std::string file_name;
			get_filename_from_full_path(pic.full_path, file_name);

			lecui::table_row row = {
				{ "Name", file_name },
				{ "Size", leccore::format_size(pic.file_size) },
				{ "Orientation", std::string(pic.orientation == image_orientation::landscape ? "Landscape" : "Portrait") }
			};

			list.data().push_back(row);
		}
	}
	catch (const std::exception&) {}

	if (_installed) {
		std::string error;
		if (!_tray_icon.add(ico_resource, std::string(appname) + " " +
			std::string(appversion) + " (" + std::string(architecture) + ")",
			{
			{ "<strong>Show Spotlight Images</strong>", [this]() {
				if (minimized())
					restore();
				else
					show();
			} },
			{ "" },
			{ "Settings", [this]() {
				add_back_button();
				_page_man.show("settings");

				if (minimized())
					restore();
				else
					show();
			}
			},
			{ "Updates", [this]() {
				if (_page_man.current() != "home") {
					// show home page
					_page_man.show("home");

					// close the back icon
					_page_man.close("status::left/back");
				}

				updates();

				if (minimized())
					restore();
				else
					show();
			} },
			{ "About", [this]() {
				add_back_button();
				_page_man.show("help");

				if (minimized())
					restore();
				else
					show();
			}
			},
			{ "" },
			{ "Exit", [this]() { close(); } }
			},
			"Show Spotlight Images", error)) {
		}
	}

	std::string error;
	// disable autodownload_updates toggle button if autocheck_updates is off
	if (_setting_autocheck_updates)
		_widget_man.enable("settings/autodownload_updates", error);
	else
		_widget_man.disable("settings/autodownload_updates", error);

	if (_installed)
		_widget_man.enable("settings/autostart", error);
	else
		_widget_man.disable("settings/autostart", error);

	_splash.remove();
}
