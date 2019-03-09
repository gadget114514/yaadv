#include "StringUtil.h"
#include <sstream>

namespace yaadv {

std::vector<std::string> StringUtil::split(const std::string &str, char delim)
{
	std::istringstream stream(str);
		
	std::string field;
	std::vector<std::string> result;
	while (std::getline(stream, field, delim)) {
		result.push_back(field);
	}
	return result;
}

std::string StringUtil::trim(const std::string& str, const char* trimCharacterList)
{
	std::string result;
	std::string::size_type left = str.find_first_not_of(trimCharacterList);
	
	if (left != std::string::npos) {
		std::string::size_type right = str.find_last_not_of(trimCharacterList);
		result = str.substr(left, right - left + 1);
	}
	
	return result;
}


}  // namespace yaadv