
#include <Logger.h>

void Logger::log(std::string_view text) {
  std::ofstream File("events.log.txt", std::ios::app);
  File << text << "\n";
  File.close();

#if defined(_DEBUG)
  std::cout << text << "\n";
  OutputDebugStringA(std::string(text).append("\n").c_str());
#endif
}

void Logger::log(std::wstring_view text) {
  std::wofstream File("events.log.txt", std::ios::app);
  File << text << L"\n";
  File.close();

#if defined(_DEBUG)
  std::wcout << text << L"\n";
  OutputDebugStringW(std::wstring(text).append(L"\n").c_str());
#endif
}

void Logger::logHR(const HRESULT hr) {

  _com_error error(hr);
  LPCTSTR errorMsg = error.ErrorMessage();

  log("Error: {}", errorMsg);
}
