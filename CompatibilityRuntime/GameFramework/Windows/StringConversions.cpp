#include <Windows/StringConversions.h>
#include <Windows/WindowsError.h>

#include <windows.h>


std::wstring codepageToWide(const std::string_view& string, unsigned int codepage) {
	if (string.empty())
		return std::wstring();

	std::wstring output;
	output.resize(string.size() * 2); // Worst case: every UTF-8 byte generates two UTF-16 wchars_t

	int characters = MultiByteToWideChar(codepage, 0, string.data(), (int)string.size(), &output[0], (int)output.size());
	if (characters == 0)
		WindowsError::throwLastError();

	output.resize(characters);

	return output;
}

std::string wideToCodepage(const std::wstring_view& string, unsigned int codepage) {
	if (string.empty())
		return std::string();

	std::string output;
	output.resize(string.size() * 4); // Worst case: every UTF-16 wchar_t generates 4 UTF-8 bytes (ignoring invalid 5, 6 byte UTF-8 sequences)

	int characters = WideCharToMultiByte(codepage, 0, string.data(), (int)string.size(), &output[0], (int)output.size(), nullptr, nullptr);
	if (characters == 0)
		WindowsError::throwLastError();

	output.resize(characters);

	return output;
}

std::wstring utf8ToWide(const std::string_view& string) {
	return codepageToWide(string, CP_UTF8);
}


std::string wideToUtf8(const std::wstring_view& string) {
	return wideToCodepage(string, CP_UTF8);
}
