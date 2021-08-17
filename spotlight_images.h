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

#include <string>
#include <vector>

enum class image_orientation {
	portrait = 0,
	landscape,
};

struct image_info {
	image_orientation orientation;
	std::string full_path;
	unsigned long long file_size = 0;
	unsigned int width = 0;
	unsigned int height = 0;
};

/// <summary>
/// Fetch Windows Spotlight images.
/// </summary>
/// 
/// <param name="folder">The folder to save the images to.</param>
/// 
/// <remarks>
/// Creates a folder within the module's directory named "Spotlight" then
/// copies Windows Spotlight images available in the current user's profile
/// into subfolders /Portrait and /Landscape depending on their orientation.
/// If images with the same names already exist they are overwritten.
/// 
/// This function uses GdiPlus. Ensure Gdiplus is initialized before calling.
/// </remarks>
/// 
/// <returns>
/// Returns a list image_info objects for all the files fetched.
/// </returns>
std::vector<image_info> fetch_images(const std::string& folder);
