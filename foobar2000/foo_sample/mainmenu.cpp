#include "stdafx.h"
#include "config.h"





void link_some(const char* baseDir, pfc::string8 name, metadb_handle_list_cref p_list);
void make_playlist(const char* baseDir, pfc::string8 name, metadb_handle_list_cref p_list);
void reset(const char* baseDir);



void link_some(const char* baseDir, pfc::string8 name, metadb_handle_list_cref p_list) {

	// get filesystem pointer
	filesystem::ptr fs = filesystem::get(baseDir);

	name = pfc::io::path::replaceIllegalNameChars(name);

	// get absolute path of the directory where files will be stored
	pfc::string8 playlistDir = baseDir;
	playlistDir.end_with(fs->pathSeparator());
	playlistDir += name;

	if (!fs->directory_exists(playlistDir, fb2k::noAbort)) {
		fs->create_directory(playlistDir, fb2k::noAbort);
	}

	t_size n, m = p_list.get_count();
	pfc::string8 inFN;
	pfc::string8 outPath;

	for (n = 0; n < m; n++) {

		// get path of source file
		const char* inPath = p_list.get_item(n)->get_path();

		// get filename + extension of source file
		fs->extract_filename_ext(inPath, inFN);

		// make a destination path
		outPath.copy(playlistDir);
		outPath.end_with(fs->pathSeparator());
		outPath += inFN;

		if (fs->exists(outPath, fb2k::noAbort)) {
			// FB2K_console_formatter() << outPath << " existed\n";
		}
		else {
			try {
				// prepend file:// to outPath
				fs->g_link(inPath, outPath, fb2k::noAbort);
			}
			catch (pfc::exception e) {
				pfc::string_formatter sf;
				throw pfc::exception(sf << "error linking " << inPath << " to " << outPath << e);

			}
		}
	}
}

// write m3u8 file
void make_playlist(const char* baseDir, pfc::string8 name, metadb_handle_list_cref p_list) {
	filesystem::ptr fs = filesystem::get(baseDir);

	name = pfc::io::path::replaceIllegalNameChars(name);

	t_size n, m = p_list.get_count();
	pfc::string8 inFN;
	pfc::string8 p_content;


	for (n = 0; n < m; n++) {
		

		const char* inPath = p_list.get_item(n)->get_path();

		fs->extract_filename_ext(inPath, inFN);

		p_content.add_string(name);
		// gotta be forward slash because .
		p_content.add_char('/');
		p_content.add_string(inFN, inFN.get_length());
		p_content.add_char('\n');
	}

	pfc::string8 playlistFn = name << ".m3u8";
	pfc::string8 outPath = baseDir;
	outPath.end_with(fs->pathSeparator());
	outPath += playlistFn;

	const double openTimeout = 1.0;
	auto outLock = file_lock_manager::get()->acquire_write(outPath, fb2k::noAbort);

	if (!p_content.is_valid_utf8()) {
		// there was an error putting utf-8 text in the m3u8 file
		// why might that be?
		throw pfc::exception("here");
	}

	size_t p_size = p_content.get_length();
	if (p_size > 0) {
		fs->rewrite_file(outPath, fb2k::noAbort, openTimeout, p_content, p_size);
	}
}

void reset(const char* baseDir) {
	filesystem::ptr fs = filesystem::get(baseDir);
	fs->g_remove(baseDir, fb2k::noAbort);
	fs->g_create_directory(baseDir, fb2k::noAbort);
}






namespace { // main menu

	static const GUID guid_mainmenu_group_id = { 0x86f3cea1, 0x57cc, 0x46ca, { 0x81, 0x7e, 0x61, 0x44, 0x8a, 0x24, 0x9e, 0xdc } };
	static const GUID guid_test = { 0x2ab57cbc, 0x415b, 0x4521, { 0x80, 0xed, 0xd5, 0x8a, 0xe6, 0x1f, 0x1c, 0x2d } };

	static mainmenu_group_factory g_mainmenu_group(guid_mainmenu_group_id, mainmenu_groups::file_playlist, mainmenu_commands::sort_priority_last);


	class mainmenu_commands_sample : public mainmenu_commands {
	public:
		t_uint32 get_command_count() override {
			return 1;
		}
		GUID get_command(t_uint32 p_index) override {
			return guid_test;
		}
		void get_name(t_uint32 p_index, pfc::string_base& p_out) override {
			p_out = "Push playlists ";
		}
		bool get_description(t_uint32 p_index, pfc::string_base& p_out) override {
			p_out << "Push playlists (as hardlink) to " << cfg_dir.get();
			return true;
		}
		GUID get_parent() override {
			return guid_mainmenu_group_id;
		}
		void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) override {


			playlist_manager::ptr playlist_manager = playlist_manager::get();

			size_t n = playlist_manager->get_playlist_count();

			


			pfc::string8 baseDir; 
			cfg_dir.get(baseDir);
			
			// idkkkkkkkkkkk
			// this only works on fb2k 2.0 and up			
			//auto baseDirRef = fb2k::configStore::get()->getConfigString("dir", default_cfg_dir);


			
				

			pfc::string_formatter errorLog;

			// get filesystem pointer
			filesystem::ptr fs = filesystem::get(baseDir);

			try {
				reset(baseDir);
			}
			catch (pfc::exception e) {
				popup_message::g_show(e.what(), "couldn't reset ... Blah");
				//return;
			}


			// is this right
			for (size_t i = 0; i < n; i++) {
				pfc::string8 name;
				playlist_manager->playlist_get_name(i, name);

				metadb_handle_list items;
				playlist_manager->playlist_get_all_items(i, items);

				try {
					make_playlist(baseDir, name, items);
				}
				catch (pfc::exception e) {
					errorLog << "error writing playlist";
				}

				// not needed
				// metadb_handle_list_helper::remove_duplicates(items);

				try {
					link_some(baseDir, name, items);
				}
				catch (pfc::exception e) {
					errorLog << "error exporting playlist " << name << ": " << e << "\n\n";
				}
			}

			if (errorLog.length() <= 0) {
				errorLog << "no error from linking";
			}
			popup_message::g_show(errorLog, "Blah");


#ifdef _WIN32
			if (!cfg_openafter.get()) return;
			if (!ModalDialogPrologue()) return;
			const HWND wndParent = core_api::get_main_window();
			uShellExecute(wndParent, "open", baseDir, NULL, NULL, SW_SHOWDEFAULT);
#endif
		}

	};

	static mainmenu_commands_factory_t<mainmenu_commands_sample> g_mainmenu_commands_sample_factory;

}
