#include "stdafx.h"

//  todo
//
//  find the directory of all the files currently in playlist
// make sure there are no duplicate directories
// copy the dir+contents of each directory to a new base location
// 
// there's a copy_dir_contents function...


//
//
//namespace {
//    const char* outPathDir = "C:\\Users\\dexte\\Sync\\testing";
//
//
//    void write_m3u8(pfc::string8 name, metadb_handle_list_cref contents) {
//        filesystem::ptr fs = filesystem::get(outPathDir);
//
//        pfc::string8 p_content;
//
//        t_size n, m = contents.get_count();
//        for (n = 0; n < m; n++) {
//
//            // see playlist_helper cpp
//            const char* inPath = contents.get_item(n)->get_path();
//
//            pfc::string8 inFN;
//            fs->extract_filename_ext(inPath, inFN);
//
//            p_content.add_string(inFN, inFN.get_length());
//            p_content.add_char('\n');
//        }
//        
//
//        
//
//        pfc::string8 playlistFn = name << ".m3u8";
//        pfc::string8 outPath = outPathDir;
//        outPath.end_with(fs->pathSeparator());
//        outPath += playlistFn;
//
//
//        const double openTimeout = 1.0;
//        auto outLock = file_lock_manager::get()->acquire_write(outPath, fb2k::noAbort);
//
//        if (!p_content.is_valid_utf8()) {
//            // there was an error putting utf-8 text in the m3u8 file
//            // why might that be?
//            throw std::exception("here");
//        }
//
//        outPath = pfc::io::path::validateFileName(outPath);
//
//
//        size_t p_size = p_content.get_length();
//
//
//
//        if (p_size > 0) {
//            try {
//                fs->rewrite_file(outPath, fb2k::noAbort, openTimeout, p_content, p_size);
//            }
//            catch (...) {
//                // something happened here
//            }
//        }
//    }
//
//    void link_some(metadb_handle_list_cref p_list) {
//        // temporary
//
//        filesystem::ptr fs = filesystem::get(outPathDir);
//
//        t_size n, m = p_list.get_count();
//        for (n = 0; n < m; n++) {
//
//            // see playlist_helper cpp
//            const char* inPath = p_list.get_item(n)->get_path();
//
//            pfc::string8 inFN;
//            fs->extract_filename_ext(inPath, inFN);
//
//            pfc::string8 outPath = outPathDir;
//            outPath.end_with(fs->pathSeparator());
//            outPath += inFN;
//
//            if (fs->exists(outPath, fb2k::noAbort)) {
//                // FB2K_console_formatter() << outPath << " existed\n";
//            }
//            else {
//                fs->g_link(inPath, outPath, fb2k::noAbort);
//            }
//        }
//    }
//
//    void delete_some(metadb_handle_list_cref p_list) {
//
//        filesystem::ptr fs = filesystem::get(outPathDir);
//
//        t_size n, m = p_list.get_count();
//        for (n = 0; n < m; n++) {
//
//            // see playlist_helper cpp
//            const char* inPath = p_list.get_item(n)->get_path();
//
//            pfc::string8 inFN;
//            fs->extract_filename_ext(inPath, inFN);
//
//            pfc::string8 outPath = outPathDir;
//            outPath.end_with(fs->pathSeparator());
//            outPath += inFN;
//
//            if (fs->exists(outPath, fb2k::noAbort)) {
//                fs->remove(outPath, fb2k::noAbort);
//            }
//            else {
//            }
//        }
//
//    }
//
//    bool overlap(metadb_handle_list_cref list_one, metadb_handle_list_cref list_two) {
//
//        t_size n, m = list_one.get_count();
//        for (n = 0; n < m; n++) {
//            auto item = list_one.get_item(n);
//
//            t_size ne, me = list_two.get_count();
//            for (ne = 0; ne < me; ne++) {
//                if (item == list_two.get_item(ne)) {
//                    console::print("dup");
//                    return true;
//                }
//            }
//        }
//
//        return false;
//    }
//
//    class my_playlist_callback : playlist_callback_impl_base {
//    public:
//        my_playlist_callback(t_uint32 p_flags) : playlist_callback_impl_base(p_flags) {}
//
//
//        void on_items_added(t_size p_playlist, t_size p_start, metadb_handle_list_cref p_data, const bit_array& p_selection) {
//            link_some(p_data);
//
//            pfc::string8 name;
//            playlist_manager::get()->playlist_get_name(p_playlist, name);
//
//            metadb_handle_list contents;
//            playlist_manager::get()->playlist_get_items(p_playlist, contents, pfc::bit_array_true());
//
//            write_m3u8(name, contents);
//
//            FB2K_console_formatter() << "rewrote playlist file: " << name << "\n";
//        }
//
//        void on_items_reordered(t_size p_playlist, const t_size* p_order, t_size p_count) {
//
//            pfc::string8 name;
//            playlist_manager::get()->playlist_get_name(p_playlist, name);
//
//            metadb_handle_list contents;
//            playlist_manager::get()->playlist_get_items(p_playlist, contents, pfc::bit_array_true());
//
//            write_m3u8(name, contents);
//
//            FB2K_console_formatter() << name;
//        }
//
//        void on_items_removing(t_size p_playlist, const bit_array& p_mask, t_size p_old_count, t_size p_new_count) {
//            console::print("removing");
//
//            pfc::string8 name;
//            playlist_manager::get()->playlist_get_name(p_playlist, name);
//
//            metadb_handle_list contents_removing;
//            playlist_manager::get()->playlist_get_items(p_playlist, contents_removing, p_mask);
//
//            metadb_handle_list contents_all_except_removing;
//            playlist_manager::get()->playlist_get_items(
//                p_playlist, contents_all_except_removing, pfc::bit_array_not(p_mask)
//            );
//
//            if (!overlap(contents_removing, contents_all_except_removing)) {
//                console::print("yipee!!!");
//                delete_some(contents_removing);
//            }
//
//            write_m3u8(name, contents_all_except_removing);
//
//            // 
//            // remove dupliates
//            // have item
//            // get item ref
//            // bsearch by pointer
//
//        }
//
//        void on_items_removed(t_size p_playlist, const bit_array& p_mask, t_size p_old_count, t_size p_new_count) {
//            console::print("removed");
//        }
//
//        void on_items_modified(t_size p_playlist, const bit_array& p_mask) {
//            console::print("modified");
//        }
//
//    };
//
//
//
//    class myinitquit : public initquit {
//    public:
//        void on_init() {
//            //PFC_ASSERT(filesystem::g_get_interface((filesystem::ptr)0, outPathDir));
//
//            filesystem::ptr fs = filesystem::get(outPathDir);
//
//            try {
//                fs->create_directory(outPathDir, fb2k::noAbort);
//            }
//            catch (...) {
//
//
//
//            }
//
//
//            //if (!fs->directory_exists(outPathDir, fb2k::noAbort)) {
//            //    popup_message::g_show("hmmm", "Information");
//            //    throw;
//
//
//            //}
//
//
//            
//            
//            // idk
//            m_cb = new my_playlist_callback(
//                playlist_callback::flag_on_items_added |
//                playlist_callback::flag_on_items_removed |
//                playlist_callback::flag_on_items_removing |
//                playlist_callback::flag_on_items_reordered |
//                playlist_callback::flag_on_items_modified
//            );
//
//            // a better way would probably be to create an abstract representation of 
//            // what the ends will look like on the file system
//
//            
//        }
//        void on_quit() {
//            console::print("Sample component: on_quit()");
//            delete m_cb;
//        }
//
//        my_playlist_callback* m_cb;
//    };
//
//    FB2K_SERVICE_FACTORY( myinitquit );
//
//} // namespace

#include <helpers/file_list_helper.h>
#include <helpers/advconfig_impl.h>





namespace {


	// settings

	static constexpr GUID guid_advconfig_branch = { 0x27324fc4, 0x4400, 0x4a9a, { 0xa7, 0x7, 0x7c, 0xba, 0x22, 0xc9, 0x9c, 0x5e } };
	static constexpr GUID guid_cfg_dir = { 0x1a4e74fd, 0xf5ef, 0x4fd0, { 0xaf, 0x24, 0x12, 0xdd, 0x11, 0x46, 0x88, 0x0 } };
	static constexpr GUID guid_cfg_openafter = { 0xbaf9456c, 0xe2fe, 0x4f0c, { 0x89, 0x8, 0x34, 0x9c, 0xa4, 0xce, 0xba, 0x90 } };


	static constexpr char default_cfg_dir[] = "C:\\Users\\dexte\\Sync\\testing2";
	static constexpr bool default_cfg_openafter = true;

	static advconfig_branch_factory g_advconfigBranch("Dex component", guid_advconfig_branch, advconfig_branch::guid_branch_tools, 0);
	static advconfig_string_factory cfg_dir("dir", guid_cfg_dir, guid_advconfig_branch, 0, default_cfg_dir);
	static advconfig_checkbox_factory cfg_openafter("open after????????", guid_cfg_openafter, guid_advconfig_branch, 1, default_cfg_openafter);

	// menu

	static const GUID guid_mainmenu_group_id = { 0x86f3cea1, 0x57cc, 0x46ca, { 0x81, 0x7e, 0x61, 0x44, 0x8a, 0x24, 0x9e, 0xdc } };
	static const GUID guid_test = { 0x2ab57cbc, 0x415b, 0x4521, { 0x80, 0xed, 0xd5, 0x8a, 0xe6, 0x1f, 0x1c, 0x2d } };

	static mainmenu_group_factory g_mainmenu_group(guid_mainmenu_group_id, mainmenu_groups::file, mainmenu_commands::sort_priority_dontcare);


	// funcs

	void link_some(const char* baseDir, pfc::string8 name, metadb_handle_list_cref p_list) {

		filesystem::ptr fs = filesystem::get(baseDir);

		name = pfc::io::path::replaceIllegalNameChars(name);

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

	void reset(const char* baseDir) {
		filesystem::ptr fs = filesystem::get(baseDir);
		fs->g_remove(baseDir, fb2k::noAbort);
		fs->g_create_directory(baseDir, fb2k::noAbort);
	}

	class mainmenu_commands_sample : public mainmenu_commands {
	public:
		t_uint32 get_command_count() override {
			return 1;
		}
		GUID get_command(t_uint32 p_index) override {
			return guid_test;
		}
		void get_name(t_uint32 p_index, pfc::string_base& p_out) override {
			p_out = "name";
		}
		bool get_description(t_uint32 p_index, pfc::string_base& p_out) override {
			p_out = "run the thing"; return true;
		}
		GUID get_parent() override {
			return guid_mainmenu_group_id;
		}
		void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) override {
			
			
			playlist_manager::ptr playlist_manager = playlist_manager::get();

			size_t n = playlist_manager->get_playlist_count();

			// get the directory all the exported playlists will be stored in
			pfc::string8 baseDir; cfg_dir.get(baseDir);

			pfc::string_formatter errorLog;

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

				// first write playlist
				// then
				// ...

				metadb_handle_list_helper::remove_duplicates(items);

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

			// open the output dir
			// make this based on a boolean option

#ifdef _WIN32
			if (!cfg_openafter.get()) return;

			if (!ModalDialogPrologue()) return;

			const HWND wndParent = core_api::get_main_window();
			//pfc::string8 copyTo;

			//if (!uBrowseForFolder(wndParent, "Choose destination folder", copyTo)) return;

			//pfc::string8 copyTo2 = PFC_string_formatter() << "file://" << copyTo;

			//RunLinkFilesHere(data, copyTo2, wndParent);


			uShellExecute(wndParent, "open", baseDir, NULL, NULL, SW_SHOWDEFAULT);
#endif
		}

		//bool get_display(t_uint32 p_index, pfc::string_base& p_text, t_uint32& p_flags) override {
		//	// OPTIONAL method
		//	bool rv = mainmenu_commands::get_display(p_index, p_text, p_flags);
		//	if (rv) switch (p_index) {
		//	case cmd_playback_stream_capture:
		//		// Add checkmark if capture is in progress
		//		if (IsCaptureRunning()) p_flags |= flag_checked;
		//		break;
		//	}
		//	return rv;
		//}
	};

	static mainmenu_commands_factory_t<mainmenu_commands_sample> g_mainmenu_commands_sample_factory;

}

namespace {

    class myinitquit : public initquit {
    public:
        void on_init() {
            // get main menu command and execute

		}
        void on_quit() {
            console::print("Sample component: on_quit()");
        }
    };

    FB2K_SERVICE_FACTORY( myinitquit );

} // namespace