#pragma once
#include <string>
#include <vector>

namespace yaadv {
class StringUtil
{
public:

	/**
	 * @see http://faithandbrave.hateblo.jp/entry/2014/05/01/171631
	 */
    static std::vector<std::string> split(const std::string& str, char delim);

	/**
	 * @see http://program.station.ez-net.jp/special/handbook/cpp/string/trim.asp
	 */
	static std::string trim(const std::string& str, const char* trimCharacterList = " \t\v\r\n");
};


}  // namespace yaadv