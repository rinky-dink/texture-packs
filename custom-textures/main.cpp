#include "methods.h"

#define FOLDER "custom" 

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
		DisableThreadLibraryCalls(hModule);

		using namespace std;
		namespace fs = std::filesystem;
		
		if (fs::exists("mods") && fs::exists("_CoreGame.asi"))
		{
			extset<fs::path> filesmods;

			if (fs::exists("mods/" FOLDER))
				filesmods.file_scanning("./mods/" FOLDER);

			if (filesmods.file_scanning("./mods"))
			{
				if (!fs::exists("mods/" FOLDER)) fs::create_directories("mods/" FOLDER);

				for (auto& e : filesmods)
				{
					string to = { "mods/" FOLDER "/" + e.filename().generic_string() };
					MoveFile(e.wstring().c_str(), wstring(to.cbegin(), to.cend()).c_str());
				}
				filesmods.pastestr();
			}
			else
			{
				if (filesmods.size() > 0)
				{
					if (!filesmods.check_settings())
						filesmods.pastestr();

				}
				else if (fs::exists("mods/gta.dat"))
					fs::rename("mods/gta.dat", "mods/gta.dat.backup");
			}

			filesmods.archive_management();
		}
		break;
	}
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}