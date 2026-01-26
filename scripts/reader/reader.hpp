#ifndef READER_HPP
#define READER_HPP
#include <gtkmm.h>
#include <vector>

class reader_component : public Gtk::Box {
	public:
		reader_component();

		// bottom settings for viewing current document
		Gtk::Box options;
			Gtk::Button prev_page;
			Gtk::Button next_page;
			
			Gtk::Label current_page;

		// container for the file contents
		Gtk::Box pages_container;
			std::vector<Gtk::Image> pages;

		// display current file path/name and back option
		Gtk::Box top_panel;
			Gtk::Button back_button;
			Gtk::Label current_path_label;

	private:
			unsigned long long current_page_index;
			unsigned long long page_count;
};


#endif // READER_HPP
