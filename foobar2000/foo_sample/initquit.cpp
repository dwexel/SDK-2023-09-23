#include "stdafx.h"

namespace {


    void write_m3u8(pfc::string8 name, metadb_handle_list_cref contents) {
        pfc::string8 p_content;

        const char* outPathDir = "C:\\Users\\dexte\\Sync\\testing";
        filesystem::ptr fs = filesystem::get(outPathDir);

        

        t_size n, m = contents.get_count();
        for (n = 0; n < m; n++) {

            // see playlist_helper cpp
            const char* inPath = contents.get_item(n)->get_path();

            pfc::string8 inFN;
            fs->extract_filename_ext(inPath, inFN);

            p_content.add_string(inFN, inFN.get_length());
            p_content.add_char('\n');
        }


        pfc::string8 outFn = name << ".m3u8";

        pfc::string8 outPath = outPathDir;
        outPath.end_with(fs->pathSeparator());
        outPath += outFn;


        const double openTimeout = 1.0;
        auto outLock = file_lock_manager::get()->acquire_write(outPath, fb2k::noAbort);

        if (!p_content.is_valid_utf8()) {
            // there was an error putting utf-8 text in the m3u8 file
            // why might that be?
            throw std::exception("here");
        }

        size_t p_size = p_content.get_length();

        if (p_size > 0) {
            try {
                fs->rewrite_file(outPath, fb2k::noAbort, openTimeout, p_content, p_size);
            }
            catch (...) {
                // something happened here
            }
        }
    }

    void link_some(metadb_handle_list_cref p_list) {
        // temporary
        const char* outPathDir = "C:\\Users\\dexte\\Sync\\testing";
        filesystem::ptr fs = filesystem::get(outPathDir);

        t_size n, m = p_list.get_count();
        for (n = 0; n < m; n++) {

            // see playlist_helper cpp
            const char* inPath = p_list.get_item(n)->get_path();

            pfc::string8 inFN;
            fs->extract_filename_ext(inPath, inFN);

            pfc::string8 outPath = outPathDir;
            outPath.end_with(fs->pathSeparator());
            outPath += inFN;

            if (fs->exists(outPath, fb2k::noAbort)) {
                // FB2K_console_formatter() << outPath << " existed\n";
            }
            else {
                fs->g_link(inPath, outPath, fb2k::noAbort);
            }
        }
    }

    void delete_some(metadb_handle_list_cref p_list) {
        // temporary
        const char* outPathDir = "C:\\Users\\dexte\\Sync\\testing";
        filesystem::ptr fs = filesystem::get(outPathDir);

        t_size n, m = p_list.get_count();
        for (n = 0; n < m; n++) {

            // see playlist_helper cpp
            const char* inPath = p_list.get_item(n)->get_path();

            pfc::string8 inFN;
            fs->extract_filename_ext(inPath, inFN);

            pfc::string8 outPath = outPathDir;
            outPath.end_with(fs->pathSeparator());
            outPath += inFN;

            if (fs->exists(outPath, fb2k::noAbort)) {
                fs->remove(outPath, fb2k::noAbort);
            }
            else {
            }
        }

    }

    bool overlap(metadb_handle_list_cref list_one, metadb_handle_list_cref list_two) {

        t_size n, m = list_one.get_count();
        for (n = 0; n < m; n++) {
            auto item = list_one.get_item(n);

            t_size ne, me = list_two.get_count();
            for (ne = 0; ne < me; ne++) {
                if (item == list_two.get_item(ne)) {
                    console::print("dup");
                    return true;
                }
            }
        }

        return false;
    }

    class my_playlist_callback : playlist_callback_impl_base {
    public:
        my_playlist_callback(t_uint32 p_flags) : playlist_callback_impl_base(p_flags) {}


        void on_items_added(t_size p_playlist, t_size p_start, metadb_handle_list_cref p_data, const bit_array& p_selection) {
            console::print("added");

            link_some(p_data);

            pfc::string8 name;
            playlist_manager::get()->playlist_get_name(p_playlist, name);

            metadb_handle_list contents;
            playlist_manager::get()->playlist_get_items(p_playlist, contents, pfc::bit_array_true());

            write_m3u8(name, contents);
        }

        void on_items_reordered(t_size p_playlist, const t_size* p_order, t_size p_count) {
            console::print("reordered");

            pfc::string8 name;
            playlist_manager::get()->playlist_get_name(p_playlist, name);

            metadb_handle_list contents;
            playlist_manager::get()->playlist_get_items(p_playlist, contents, pfc::bit_array_true());

            write_m3u8(name, contents);
        }

        void on_items_removing(t_size p_playlist, const bit_array& p_mask, t_size p_old_count, t_size p_new_count) {
            console::print("removing");



            pfc::string8 name;
            playlist_manager::get()->playlist_get_name(p_playlist, name);

            metadb_handle_list contents_removing;
            playlist_manager::get()->playlist_get_items(p_playlist, contents_removing, p_mask);


            metadb_handle_list contents_all_except_removing;
            playlist_manager::get()->playlist_get_items(
                p_playlist, contents_all_except_removing, pfc::bit_array_not(p_mask)
            );




            if (!overlap(contents_removing, contents_all_except_removing)) {
                console::print("yipee!!!");
                delete_some(contents_removing);
            }

            write_m3u8(name, contents_all_except_removing);

            // 
            // remove dupliates
            // have item
            // get item ref
            // bsearch by pointer

        }

        void on_items_removed(t_size p_playlist, const bit_array& p_mask, t_size p_old_count, t_size p_new_count) {
            console::print("removed");
        }

        void on_items_modified(t_size p_playlist, const bit_array& p_mask) {
            console::print("modified");
        }

    };

    class myinitquit : public initquit {
    public:
        void on_init() {
            console::print("Sample component: on_init()");

            // idk
            m_cb = new my_playlist_callback(
                playlist_callback::flag_on_items_added |
                playlist_callback::flag_on_items_removed |
                playlist_callback::flag_on_items_removing |
                playlist_callback::flag_on_items_reordered |
                playlist_callback::flag_on_items_modified
            );
        }
        void on_quit() {
            console::print("Sample component: on_quit()");
            delete m_cb;
        }

        my_playlist_callback* m_cb;
    };

    FB2K_SERVICE_FACTORY( myinitquit );

} // namespace
