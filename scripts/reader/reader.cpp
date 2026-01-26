#include "reader.hpp"
#include <cstdio>
#include <gtkmm.h>

#include <iostream>
#include <string>

reader_component::reader_component() :
	Gtk::Box(Gtk::Orientation::VERTICAL),

	pages_container(),

	// top panel
	top_panel(Gtk::Orientation::HORIZONTAL),
	back_button("<"),
	current_path_label("~/books/stuff/doc.pdf"),

	// options
	options(Gtk::Orientation::HORIZONTAL),
	prev_page("<-"),
	next_page("->"),
	current_page("loading...")
{
	// top panel
	top_panel.set_size_request(-1, 20);
	current_path_label.set_hexpand(true);
	back_button.set_size_request(35, -1);

	top_panel.append(back_button);
	top_panel.append(current_path_label);
	append(top_panel);
	// pages
	pages_container.set_expand(true);
	append(pages_container);
	
	// options
	options.set_size_request(-1, 50);

	//std::string display_p_number = current_page_index + " / " + page_count;
	current_page.set_text("1 / 100");

	prev_page.set_hexpand(true);
	next_page.set_hexpand(true);
	current_page.set_hexpand(true);
	options.append(prev_page);
	options.append(current_page);
	options.append(next_page);
	append(options);


};

