#ifndef READER_HPP
#define READER_HPP
#include <cstdint>
#include <gtkmm.h>
#include <gdkmm.h>
#include <vector>

class reader_component : public Gtk::Box {
	public: // UI stuff goes here
		reader_component();
		Gtk::DrawingArea test;

		// bottom settings for viewing current document
		Gtk::Box options;
			Gtk::Button prev_page;
			Gtk::Button next_page;
			
			Gtk::Label current_page;

		// container for the file contents
		Gtk::Box pages_container;
			std::vector<Gtk::DrawingArea> pages;

		// display current file path/name and back option
		Gtk::Box top_panel;
			Gtk::Button back_button;
			Gtk::Label current_path_label;

	private: // stuff related to processing the file goes here
		uint64_t current_page_index;
		uint64_t page_count;

		std::vector<Glib::RefPtr<Gdk::Pixbuf>> page_pixmaps;
		// for the pages drawingarea
		void draw_page(const Cairo::RefPtr<Cairo::Context>& cr, Glib::RefPtr<Gdk::Pixbuf> buff, int width, int height);
		
		void load_file(std::string path);
			
		// used to load each page by itself in a specific thread
		void * page_render_thread(void *data_);

		void build_pages_ui();

};


#endif // READER_HPP
