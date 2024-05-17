#include "stdafx.h"
#include <helpers/file_list_helper.h>
#include <helpers/filetimetools.h>
#include <memory>


void link_some(const char* baseDir, pfc::string8 name, metadb_handle_list_cref p_list);



void RunLinkFilesHere(metadb_handle_list_cref data, const char* copyTo, fb2k::hwnd_t wndParent) {
	// can't use the "data" parameter because it doesn't separate tracks by their playlist's name,
	// but 
	// it's possible to run a context command on something that's not in a playlist
	// hm

	playlist_manager::ptr pm = playlist_manager::get();
	size_t n = pm->get_playlist_count();
	pfc::string_formatter errorLog;

	// is this right
	for (size_t i = 0; i < n; i++) {
		pfc::string8 name;
		pm->playlist_get_name(i, name);

		metadb_handle_list items;
		pm->playlist_get_selected_items(i, items);

		if (items.size() > 0) 
		{
			// remember about writing playlist first

			// metadb_handle_list_helper::remove_duplicates(items);

			try {
				link_some(copyTo, name, items);
			}
			catch (pfc::exception e) {
			}

		}
	}
}


void RunLinkFiles(metadb_handle_list_cref data) {

#ifdef _WIN32
	if (!ModalDialogPrologue()) return;
	const HWND wndParent = core_api::get_main_window();
	pfc::string8 copyTo;
	if (!uBrowseForFolder(wndParent, "Choose destination folder", copyTo)) return;
	pfc::string8 copyTo2 = PFC_string_formatter() << "file://" << copyTo;
	RunLinkFilesHere(data, copyTo2, wndParent);
#else
	auto tracksCopy = std::make_shared<metadb_handle_list>(data);
	auto setup = fb2k::fileDialog::get()->setupOpenFolder();
	setup->setTitle("Choose destination folder");
	setup->runSimple([tracksCopy](fb2k::stringRef path) {
		RunCopyFilesHere(*tracksCopy, path->c_str(), core_api::get_main_window());
		});

#endif
}

