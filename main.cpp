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

#include <liblec/cui/gui.h>
using namespace liblec::cui;

#include <algorithm>

#include "resource.h"
#include "spotlight_images.h"
#include "helper_functions.h"
#include "version_info.h"

// gui app using main
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

class spotlight_images : public gui {
	const std::string home_page_name_;
	const liblec::cui::size form_size_{ 800, 500 };
	const long margin_ = 10;
	const long icon_size_ = 32;
	const long info_size_ = 20;
	std::vector<image_info> pictures_;
	image_info displayed_image_;

	void on_caption() override {
		gui::prompt_params params;
		params.type = gui::prompt_type::ok;

		std::string display_text = std::string(appname) + " " + std::string(appversion) + " " + std::string(appdate);

		display_text += "\n\nFor more info visit:\nhttps://github.com/alecmus/spotlight_images";
		display_text += "\n\nIcons designed by Freepik:\nhttp://flaticon.com";

		display_text += "\n\nLibraries used:";
		display_text += "\n" + version();

		prompt(params, "", display_text);
	}

	void on_stop() override {
		close();
	}

	void on_shutdown() override {}

	void on_run() override {
		pictures_ = fetch_images();

		std::string error;

		// display caption
		std::string message = std::to_string(pictures_.size()) + " image";
		if (pictures_.size() != 1) message += "s";

		message += " copied.";

		if (pictures_.size() == 0) message = "No images were copied.";

		if (pictures_.size() != 0) {
			int landscape = std::count_if(pictures_.begin(), pictures_.end(),
				[](image_info p) {
					return p.orientation == image_orientation::landscape;
				});

			message += " " + std::to_string(landscape) + " Landscape, " + std::to_string(pictures_.size() - landscape) + " Portrait.";
			message += " Select to preview.";
		}

		set_text(home_page_name_ + "/caption", message, error);

		// populate listview
		for (const auto& pic : pictures_) {
			std::string file_name;
			get_filename_from_full_path(pic.full_path, file_name);

			widgets::listview_row row =
			{ { { {"Name"}, {file_name} },
				{ {"Size"}, {format_size(pic.file_size)} },
				{ {"Orientation"}, { pic.orientation == image_orientation::landscape ? "Landscape" : "Portrait" } } } };

			add_listview_row(home_page_name_ + "/list", row, true, error);
		}
	}

public:
	spotlight_images() :
		home_page_name_("Spotlight Images")	{}
	~spotlight_images() {}

	bool layout(gui::page& persistent_page,
		gui::page& home_page,
		std::string& error) override {
		// set caption
		home_page.set_name(home_page_name_);

		// set form parameters
		prevent_resizing();

		// set form dimensions
		set_width(form_size_.width);
		set_min_width(form_size_.width);
		set_height(form_size_.height);
		set_min_height(form_size_.height);

		// set up form icons
		set_icons(ico_resource,
			gui::caption_icon_png{ icon_16, icon_20, icon_24, icon_28, icon_32 });

		// add caption
		widgets::text caption;
		caption.alias = "caption";
		caption.color = { 130, 130, 130 };
		caption.rect = { margin_, (long)width() - margin_, margin_, margin_ + 20 };
		caption.alignment = widgets::text_alignment::center;
		home_page.add_text(caption);

		// add listview
		widgets::listview list;
		list.alias = "list";
		list.border = false;
		list.rect = { margin_, ((long)width() / 2), caption.rect.bottom + margin_, (long)height() - margin_ };
		list.columns = {
			{ "Name", 240, widgets::listview_column_type::string_ },
			{ "Size", 50, widgets::listview_column_type::string_ },
			{ "Orientation", 90, widgets::listview_column_type::string_ }
		};
		list.unique_column_name = "Name";
		list.on_selection = [&]() {
			displayed_image_.full_path.clear();

			try {
				std::string filename, error;
				std::vector<widgets::listview_row> rows;
				get_listview_selected(home_page_name_ + "/list", rows, error);

				if (rows.size() == 1) {
					for (auto& item : rows[0].items) {
						if (item.column_name == "Name") {
							filename = item.item_data;
							break;
						}
					}

					if (!filename.empty()) {
					}
				}

				for (const auto& it : pictures_) {
					std::string filename_;
					get_filename_from_full_path(it.full_path, filename_);
					if (filename == filename_) {
						displayed_image_ = it;
						break;
					}
				}

				if (!displayed_image_.full_path.empty()) {
					change_image(home_page_name_ + "/image", displayed_image_.full_path, true, error);

					std::string file_info = "Resolution: " + std::to_string(displayed_image_.width) + "x" + std::to_string(displayed_image_.height);
					file_info += ", Size: " + format_size(displayed_image_.file_size);
					set_text(home_page_name_ + "/file_info", file_info, error);
				}
				else {
					change_image(home_page_name_ + "/image", -1, true, error);
					set_text(home_page_name_ + "/file_info", "", error);
				}
			}
			catch (const std::exception& e) {
				// to-do: log error
			}
		};

		home_page.add_listview(list);

		// add image
		widgets::image image;
		image.alias = "image";
		image.rect = { list.rect.right + margin_, (long)width() - margin_, list.rect.top, list.rect.bottom - icon_size_ - info_size_ - margin_ };
		image.on_click = [&]() {
			if (!displayed_image_.full_path.empty())
				ShellExecuteA(0, 0, displayed_image_.full_path.c_str(), 0, 0, SW_SHOW);
		};

		home_page.add_image(image);

		// add info
		widgets::text file_info;
		file_info.alias = "file_info";
		file_info.color = { 150, 150, 150 };
		file_info.font_size = 8;
		file_info.rect = { image.rect.left, image.rect.right, image.rect.bottom + margin_, image.rect.bottom + margin_ + 20 };
		file_info.alignment = widgets::text_alignment::center;
		file_info.text_value = "";

		home_page.add_text(file_info);

		// add icons
		const rect rc_ref = { image.rect.left, image.rect.right, file_info.rect.bottom, list.rect.bottom };
		widgets::image open_file;
		open_file.alias = "open_file";
		open_file.tooltip = "Open file";
		open_file.png_resource = png_open_image;
		open_file.rect = { 0, icon_size_, 0, icon_size_ };
		tools::pos_rect(rc_ref, open_file.rect, 40, 50);
		open_file.on_click = [&]() {
			if (!displayed_image_.full_path.empty())
				ShellExecuteA(0, 0, displayed_image_.full_path.c_str(), 0, 0, SW_SHOW);
		};

		home_page.add_image(open_file);

		widgets::image open_folder;
		open_folder.alias = "open_folder";
		open_folder.tooltip = "Open containing folder";
		open_folder.png_resource = png_open_folder;
		open_folder.rect = { 0, icon_size_, 0, icon_size_ };
		tools::pos_rect(rc_ref, open_folder.rect, 60, 50);
		open_folder.on_click = [&]() {
			if (!displayed_image_.full_path.empty()) {
				std::string directory;
				get_directory_from_full_path(displayed_image_.full_path, directory);
				if (!directory.empty())
					ShellExecuteA(0, 0, directory.c_str(), 0, 0, SW_SHOW);
			}
		};

		home_page.add_image(open_folder);

		return true;
	}

};

int main() {
	// constants used by the app
	const std::string guid_app = "{24DE7949-0EB7-4086-85F0-76D10191E633}";	// allow only one instance of the app
	const std::string guid_form = "{EFA84F43-D48D-475A-A826-915FC65687A1}";	// to bring existing form to the foreground if a new instance is attempted

	spotlight_images gui_app;

	std::string error;
	if (!gui_app.run(guid_form, error)) {
		gui::prompt_params params;
		params.type = gui::prompt_type::ok;
		params.png_icon_resource = png_error;
		gui_app.prompt(params, "Error", error);
		return 1;
	}

	return 0;
}