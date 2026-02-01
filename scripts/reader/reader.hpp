#ifndef READER_HPP
#define READER_HPP
#include "gtkmm/drawingarea.h"
#include <gtkmm.h>
#include <gdkmm.h>
#include <mupdf/fitz.h>

#include <vector>

class page_data {
	public:
		page_data();
		~page_data();

		// mupdf stuff
		fz_context *ctx;
		int page_number;
		fz_display_list *list;
		fz_rect bbox;
		fz_pixmap *pix;
		int failed;

		// gtk stuff
		Gtk::DrawingArea widget;
		Glib::RefPtr<Gdk::Pixbuf> w_pix;

};

class reader_component : public Gtk::Box {
	public: // UI stuff goes here
		reader_component();

		// bottom settings for viewing current document
		Gtk::Box options;
			Gtk::Button prev_page;
			Gtk::Button next_page;
			
			Gtk::Label current_page;

		// container for the file contents
		Gtk::Box pages_container;
			std::vector<Gtk::DrawingArea> pages;
			//std::vector<page_data> pages;

		// display current file path/name and back option
		Gtk::Box top_panel;
			Gtk::Button back_button;
			Gtk::Label current_path_label;
		

	private: // stuff related to processing the file goes here
		uint64_t current_page_index;
		uint64_t page_count;

		std::vector<Glib::RefPtr<Gdk::Pixbuf>> page_pixmaps;
		
		void load_file(std::string path);
			
		// used to load each page by itself in a specific thread
		void * page_render_thread(void *data_);
		
		// ui related functions
		void build_pages_ui();

		void set_page(int index);

		void set_next_page();
		void set_prev_page();
};


#endif // READER_HPP
