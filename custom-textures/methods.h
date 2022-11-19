#pragma once
#include <string>
#include <filesystem>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <regex>
#include <Windows.h>
#include <set>

using namespace std;
namespace fs = std::filesystem;

#define FOLDER "custom" 

string toupper(string str)
{
	for (auto& c : str) c = toupper(c);
	return str;
}

struct ssort
{
	bool operator () (const fs::path& ls, const fs::path& rs) const
	{
		return ::toupper(ls.extension().generic_string()) < ::toupper(rs.extension().generic_string());
	}
};

string format_str(const fs::path p)
{
	string write{ p.extension().generic_string().substr(1,3) + " mods/" FOLDER "/" + p.filename().generic_string() };
	transform(write.cbegin(), write.cend(), write.begin(), [](unsigned char c) { return toupper(c); });
	return write;
}

template<typename T>
class extset : public multiset<T, ssort> {
	vector <string> imgs{ "cutscene", "gta3", "gta3_def", "gta3_def2", "gta3_def_autumn", "gta3_def_season_base", "gta3_def_summer", "gta3_def_winter", "gta3_def_winter2", "gta_int", "player" };
public:
	extset() {}
	extset(initializer_list<T> l) : multiset<T, ssort>(l) {}
	bool file_scanning(const char* path)
	{
		bool added{};
		vector <string> extension{ "IMG", "IDE", "IPL" };
		for (auto& file : fs::directory_iterator(path))
		{
			if (file.is_directory()) continue;

			fs::path p{ file };

			if (find_if(imgs.cbegin(), imgs.cend(), [&](const string& str) { return (str + ".img") == p.filename().generic_string(); }) != imgs.cend())
				continue;

			if (find_if(extension.cbegin(), extension.cend(), [&](const string& str) { return ::toupper(p.extension().generic_string()) == ("." + str); }) == extension.cend())
				continue;

			added = true;
			this->insert(p);
		}
		return added;
	}

	void archive_management()
	{
		bool status{ find_if(imgs.cbegin(), imgs.cend(), [&](const string& str) {
			string sf{ format("mods/{}.img",str) };
			return (fs::exists(sf) && fs::file_size(sf) != 8) || this->size() > 0;
			}) != imgs.cend()
		};

		for (auto& e : imgs)
		{
			if (e == "player")
				continue;

			string sf{ format("mods/{}.img",e) };
			if (status)
			{
				if (!fs::exists(sf))
				{
					ofstream file(sf, ios::out | ios::binary);
					file.write("VER2", 8);
					file.close();
				}
			}
			else
				if (fs::exists(sf) && fs::file_size(sf) == 8)
					fs::remove(sf);
		}
	}


	void pastestr()
	{
		fstream file{ "mods/gta.dat", ios::out }, gta_dat("data/gta.dat", ios::in);
		if (file.good() && gta_dat.good())
		{
			string str;
			for (; getline(gta_dat, str), str != ""; file << str << endl);
			file << endl;

			auto i = find_if(this->cbegin(), this->cend(), [&](fs::path p) {
				if (::toupper(p.extension().generic_string()) == ".IPL")
					return true;

				if (*this->cbegin() == p)
					file << "# " FOLDER "\n";

				file << format_str(p) << endl;
				return false;
				});

			if (i != this->cbegin())
				file << "# " FOLDER "\n\n";

			for (; getline(gta_dat, str); file << str << endl);

			if (i != this->cend())
			{
				file << "\n# " FOLDER "\n";
				for (; i != this->end(); ++i)
					file << format_str(*i) << endl;
				file << "# " FOLDER;
			}
		}
		file.close();
		gta_dat.close();
	}

	bool check_settings() {
		ifstream gta_dat("mods/gta.dat", ios::in);
		auto files = this->cbegin();
		string str;
		if (gta_dat.good()) {
			for (size_t i = 0; i < 2; i++) {
				if (i == 0)
					gta_dat.seekg(68, ios::beg);
				else
				{
					int_fast32_t pos{ 2 + 2 * string("# " FOLDER).size() };

					for (auto j = files; j != this->cend(); j++)
						pos += fs::path{ *j }.filename().generic_string().size() + string(FOLDER).size() + 12;

					gta_dat.seekg(-pos, ios::end);
				}
				getline(gta_dat, str);

				if (str == "# " FOLDER) {
					while (getline(gta_dat, str) && str != "# " FOLDER) {
						if (files == this->end()) return false;

						if (str == format_str(*files))
							++files;
						else
						{
							gta_dat.close();
							return false;
						}
					}
				}
			}
		}
		gta_dat.close();
		return files == this->cend();
	}
};