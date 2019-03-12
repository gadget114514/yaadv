#include <fstream>
#include <iostream>
#include <xlnt/xlnt.hpp>

bool starts_with(const std::string& s, const std::string& prefix) {
   auto size = prefix.size();
   if (s.size() < size) return false;
   return std::equal(std::begin(prefix), std::end(prefix), std::begin(s));
}

std::string rep(std::string ss, std::string target, std::string replacement)
{
	std::string s = ss;
if (!target.empty()) {
  std::string::size_type pos = 0;
  while ((pos = s.find(target, pos)) != std::string::npos) {
    s.replace(pos, target.length(), replacement);
    pos += replacement.length();
  }
	return "\"" + ss + "\"";
} else {
	return s;
}
}

std::string csvcell(std::string s)
{
	if (s.find("\"") != std::string::npos) {
		s = rep(s, "\"", "\\\"");
	}

	return s;
}

int csv(xlnt::worksheet &ws, std::string title)
{
    std::clog << "Processing spread sheet: " << title << std::endl;

    std::string fname = title + ".csv"; 
    std::ofstream ofs(fname);
    	

    for (auto row : ws.rows(false)) 
    {
	int flag = 0; 
	int ncell = 0;
        for (auto cell : row) 
	{ 
		if (flag) {
			ofs << ",";
		}
	    std::string s = cell.to_string() ;
	if (ncell == 0 && starts_with(s, "#")) {
		continue;
	}
		ofs << csvcell(s);
            flag = 1;
	ncell++;
	}
			ofs << "\n";
    }
    std::clog << "Processing complete: " << title << std::endl;

   return 0;
}

int main()
{
    xlnt::workbook wb;
    wb.load("game.xlsx");

    std::vector<std::string> titles = wb.sheet_titles();

    for (auto x: titles) {
	  std::clog << "sheet " << x << std::endl;
	xlnt::worksheet ws = wb.sheet_by_title(x);
	csv(ws, x);
    }
    return 0;
}
