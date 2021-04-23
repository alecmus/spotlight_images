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

#include <iostream>
#include <string>
#include <filesystem>
#include <Windows.h>
#include <tchar.h>
#include <ShlObj.h>

#include <GdiPlus.h>
#pragma comment(lib, "GdiPlus.lib")

/// <summary>
/// Get current module's full path, whether it's a .exe or a .dll.
/// </summary>
/// 
/// <param name="full_path">
/// Full path to the module.
/// </param>
/// 
/// <returns>
/// Returns true if successful, else false;
/// </returns>
bool get_module_full_path(std::string& full_path) {
	char buffer[MAX_PATH];
	HMODULE h_module = NULL;

	if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		// use the address of the current function to detect the module handle of the current application (critical for DLLs)
		(LPCSTR)&get_module_full_path,
		&h_module))
		return false;

	// using the module handle enables detection of filename even of a DLL
	GetModuleFileNameA(h_module, buffer, MAX_PATH);
	full_path = buffer;
	return true;
}

/// <summary>
/// Extract directory from full path.
/// </summary>
/// 
/// <param name="full_path">
/// The full path, e.g. C:\\MyFolder\\myFile.txt
/// </param>
/// 
/// <param name="directory">
/// The directory in which file specified in full_path is.
/// </param>
/// 
/// <returns>
/// Returns true if successful, else false.
/// </returns>
bool get_directory_from_full_path(
	const std::string& full_path,
	std::string& directory) {
	directory.clear();

	const size_t last_slash_index = full_path.rfind('\\');

	if (std::string::npos != last_slash_index)
		directory = full_path.substr(0, last_slash_index);

	return true;
}

/// <summary>
/// Extract file name from full path.
/// </summary>
/// 
/// <param name="sFullPath">
/// The full path, e.g. C:\\MyFolder\\myFile.txt
/// </param>
/// 
/// <param name="sFileName">
/// The directory in which file specified in sFullPath is.
/// </param>
/// 
/// <returns>
/// Returns true if successful, else false.
/// </returns>
bool get_filename_from_full_path(
	const std::string& full_path,
	std::string& file_name) {
	file_name.clear();

	const size_t last_slash_idx = full_path.rfind('\\');

	if (std::string::npos != last_slash_idx) {
		file_name = full_path;
		file_name.erase(0, last_slash_idx + 1);
	}

	return true;
}

std::string get_current_folder() {
	std::string full_path;
	if (get_module_full_path(full_path)) {
		std::string current_folder;
		return get_directory_from_full_path(full_path, current_folder) ?
			current_folder : std::string();
	}
	else
		return std::string();
}

/*
** application auto manage class (for managing critical application resources that
** need to be started at application startup and destroyed at application shutdown;
** GDI+ in this case)
** create one and only one object of this class at the start of the application
** the destructor will call all the required cleanup procedures
*/
class auto_manage {
public:
	auto_manage() {
		// initialize GDI+
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		GdiplusStartup(&gdiplus_token, &gdiplusStartupInput, NULL);
	}

	~auto_manage() {
		// shut down GDI+
		Gdiplus::GdiplusShutdown(gdiplus_token);
	}

private:
	ULONG_PTR gdiplus_token;
};

int main() {
	auto_manage auto_manage_object;

	auto get_app_data_folder = []() {
		CHAR szPath[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szPath))) {
			/*
			** C:\Users\<username>\AppData\ (Vista onwards) or
			** C:\Documents and Settings\<username>\AppData\ (XP)
			*/
			return std::string(szPath);
		}
		else
			return std::string();
	};

	// get Windows spotlight directory for current user
	std::string path = get_app_data_folder() +
		"\\Packages\\Microsoft.Windows.ContentDeliveryManager_cw5n1h2txyewy\\LocalState\\Assets";

	// get the list of files in the spotlight folder
	std::vector<std::filesystem::path> file_list;
	for (const auto& entry : std::filesystem::directory_iterator(path))
		if (entry.is_regular_file())
			file_list.push_back(entry.path());

	// get current folder
	std::string current_folder = get_current_folder();

	int copied_images = 0;

	try {
		// eliminate files that don't make sense
		for (auto& it : file_list) {
			// get path
			std::string sPath = it.string();

			// create GDI+ bitmap from file
			Gdiplus::Bitmap* p_gdibitmap;
			p_gdibitmap = new Gdiplus::Bitmap(std::basic_string<TCHAR>(sPath.begin(), sPath.end()).c_str());

			// get status information
			Gdiplus::Status status = p_gdibitmap->GetLastStatus();

			// check if an error occured
			if (status != 0)
			{
				// delete bitmap from memory
				if (p_gdibitmap) {
					delete p_gdibitmap;
					p_gdibitmap = NULL;
				}
			}
			else {
				// skip square images
				if (p_gdibitmap->GetWidth() == p_gdibitmap->GetHeight())
					continue;

				// get file name
				std::string file_name;
				get_filename_from_full_path(it.string(), file_name);

				// create new folder
				std::string new_folder = current_folder + "\\Spotlight Images";

				try {
					// if the "Windows SpotLight' folder doesn't exist, create it
					std::filesystem::create_directory(new_folder);

					std::string new_file = new_folder + "\\" + file_name + ".jpg";

					// if the file exists, delete it
					std::filesystem::directory_entry path(new_file);

					if (path.exists())
						std::remove(new_file.c_str());

					// save the image to the new file with the .jpg extension
					std::filesystem::copy_file(it, new_file);
					copied_images++;
				}
				catch (const std::exception& e) {
					std::cout << e.what() << std::endl;
				}

				// delete bitmap from memory
				if (p_gdibitmap) {
					delete p_gdibitmap;
					p_gdibitmap = NULL;
				}
			}
		}
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return 0;
	}

	std::string message = std::to_string(copied_images) + " image";
	if (copied_images != 1) message += "s";

	message += " copied successfully!";

	if (copied_images == 0) message = "No images were copied.";

	std::cout << ">>>>> " << message << " <<<<<" << std::endl << std::endl;
	system("pause");
	return 0;
}