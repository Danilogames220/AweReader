#include "./reader.hpp"
#include "gdkmm/general.h"
#include "gdkmm/pixbuf.h"
#include "gtkmm/drawingarea.h"
#include "gtkmm/enums.h"
#include "mupdf/fitz.h"

#include <gtkmm.h>

#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <format>

reader_component::reader_component() :
	Gtk::Box(Gtk::Orientation::VERTICAL),

	// top panel
	top_panel(Gtk::Orientation::HORIZONTAL),
	back_button("<"),
	current_path_label("~/books/stuff/doc.pdf"),

	// pages
	pages_container(Gtk::Orientation::VERTICAL),

	// options
	options(Gtk::Orientation::HORIZONTAL),
	prev_page("<-"),
	next_page("->"),
	current_page("loading...")
{
	current_page_index = 0;

	std::thread load_file_t([this](){
		load_file("./doc.pdf");

		// ui changes related to file loading are only safe inside here
		Glib::signal_idle().connect_once([this]() {
        	build_pages_ui();
		
			current_page.set_text(std::format("{} / {}", current_page_index + 1, page_count));
			
			// connect signals to buttons
			next_page.signal_clicked().connect(
				sigc::mem_fun(*this, &reader_component::set_next_page)
			);
			prev_page.signal_clicked().connect(
				sigc::mem_fun(*this, &reader_component::set_prev_page)
			);

   		});

	});
	load_file_t.detach();

	// load ui

	// top panel
	top_panel.set_size_request(-1, 20);
	current_path_label.set_hexpand(true);
	back_button.set_size_request(35, -1);

	top_panel.append(back_button);
	top_panel.append(current_path_label);
	append(top_panel);
	// pages
	pages_container.set_expand(true);
	
	//pages_container.append(test);
	append(pages_container);

	// options
	options.set_size_request(-1, 50);

	//current_page.set_text("1 / 100");

	prev_page.set_hexpand(true);
	next_page.set_hexpand(true);
	current_page.set_hexpand(true);
	options.append(prev_page);
	options.append(current_page);
	options.append(next_page);
	append(options);
	

};

void reader_component::build_pages_ui() {
    for (int i = 0; i < page_pixmaps.size(); i++) {

		pages.push_back(Gtk::DrawingArea());
        Gtk::DrawingArea * da = &pages[i];

        da->set_content_width(pages_container.get_width());
        da->set_content_height(pages_container.get_height());
	
        da->set_draw_func([this, i](const Cairo::RefPtr<Cairo::Context>& cr, int width, int height){
                Gdk::Cairo::set_source_pixbuf(cr, page_pixmaps[i]);
                cr->paint();
            }
        );

        if (i != current_page_index) {
            da->hide();
		}

        pages_container.append(*da);
    }
};

void reader_component::set_page(int index) {
	std::cout << "show_page ran; index =" << index << "\n";
	if (index >= page_count || index < 0) {
		return;
	};

	pages[current_page_index].hide();
	current_page_index = index;
	current_page.set_text(std::format("{} / {}", current_page_index + 1, page_count));
	pages[current_page_index].show();

	
};

void reader_component::set_next_page() {
	set_page(current_page_index + 1);
};
void reader_component::set_prev_page() {
	set_page(current_page_index - 1);
};
