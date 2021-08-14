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
#include <liblec/lecui/widgets/image_view.h>

// leccore
#include <liblec/leccore/system.h>

bool main_form::on_layout(std::string& error) {
	auto& home = _page_man.add("home");

	// add caption
	auto& caption = lecui::widgets::label::add(home, "caption");
	caption
		.color_text(lecui::color().red(100).green(100).blue(100))
		.rect(lecui::rect().left(_margin).top(_margin).right(home.size().get_width() - _margin).height(20.f))
		.center_h(true);

	// add table view
	auto& list = lecui::widgets::table_view::add(home, "list");
	list
		.border(.0f)
		.color_fill(lecui::color().alpha(0))
		.color_fill_alternate(lecui::color().alpha(0))
		.color_fill_header(lecui::color().alpha(0))
		.grid_line(.2f)
		.rect(lecui::rect().left(_margin).right(home.size().get_width() / 2.f).top(caption.rect().bottom() + _margin).bottom(home.size().get_height() - _margin))
		.fixed_number_column(true)
		.user_sort(true)
		.columns({
			{ "Name", 190 },
			{ "Size", 50 },
			{ "Orientation", 90 }
			}
		)
		.events().selection = [this](const std::vector<lecui::table_row>& rows) {
		_displayed_image.full_path.clear();
		std::string filename;

		if (rows.size() == 1) {
			try {
				filename = lecui::get::text(rows[0].at("Name"));

				for (const auto& it : _pictures) {
					std::string filename_;
					get_filename_from_full_path(it.full_path, filename_);
					if (filename == filename_) {
						_displayed_image = it;
						break;
					}
				}

				auto& image = get_image_view("home/image");
				auto& file_info = get_label("home/file_info");

				if (!_displayed_image.full_path.empty()) {
					image.file(_displayed_image.full_path);

					std::string file_info_text = "Resolution: " + std::to_string(_displayed_image.width) + "x" + std::to_string(_displayed_image.height);
					file_info_text += ", Size: " + format_size(_displayed_image.file_size);
					
					file_info.text(file_info_text);
				}
				else {
					image.file("");
					file_info.text("");
				}

				update();
			}
			catch (const std::exception&) {}
		}
	};

	// add image
	auto& image = lecui::widgets::image_view::add(home, "image");
	image
		.rect(lecui::rect()
			.left(list.rect().right() + _margin)
			.right(home.size().get_width() - _margin)
			.top(list.rect().top())
			.bottom(list.rect().bottom() - _icon_size - _info_size - _margin)
		)
		.events().action = [&]() {
		if (!_displayed_image.full_path.empty()) {
			std::string error;
			if (!leccore::shell::open(_displayed_image.full_path, error))
				message(error);
		}
	};

	// add info
	auto& file_info = lecui::widgets::label::add(home, "file_info");
	file_info
		.color_text(lecui::color().red(150).green(150).blue(150))
		.font_size(8.f)
		.center_h(true)
		.rect(lecui::rect()
			.left(image.rect().left())
			.right(image.rect().right())
			.top(image.rect().bottom() + _margin)
			.height(20.f));

	// add icons
	const auto rc_ref =  lecui::rect()
		.left(image.rect().left())
		.right(image.rect().right())
		.top(file_info.rect().bottom())
		.bottom(list.rect().bottom());

	auto& open_file = lecui::widgets::image_view::add(home);
	open_file
		.tooltip("Open file")
		.png_resource(png_open_image)
		.rect(lecui::rect()
			.size(_icon_size, _icon_size)
			.place(rc_ref, 40.f, 50.f))
		.events().action = [this]() {
		if (!_displayed_image.full_path.empty()) {
			std::string error;
			if (!leccore::shell::open(_displayed_image.full_path, error))
				message(error);
		}
	};

	auto& open_folder = lecui::widgets::image_view::add(home);
	open_folder
		.tooltip("Open containing folder")
		.png_resource(png_open_folder)
		.rect(lecui::rect()
			.size(_icon_size, _icon_size)
			.place(rc_ref, 60.f, 50.f))
		.events().action = [&]() {
		if (!_displayed_image.full_path.empty()) {
			std::string directory;
			get_directory_from_full_path(_displayed_image.full_path, directory);
			if (!directory.empty()) {
				std::string error;
				if (!leccore::shell::open(directory, error))
					message(error);
			}
		}
	};

	_page_man.show("home");
	return true;
}
