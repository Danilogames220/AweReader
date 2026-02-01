#include "./reader.hpp"
#include "gdkmm/general.h"
#include "gdkmm/pixbuf.h"
#include "gtkmm/enums.h"
#include "mupdf/fitz.h"

#include <cstdio>
#include <gtkmm.h>

#include <iostream>
#include <string>
#include <memory>
#include <thread>


reader_component::reader_component() :
	Gtk::Box(Gtk::Orientation::VERTICAL),

	// top panel
	top_panel(Gtk::Orientation::HORIZONTAL),
	back_button("<"),
	current_path_label("~/books/stuff/doc.pdf"),

	// pages
	pages_container(Gtk::Orientation::VERTICAL),
	test(),

	// options
	options(Gtk::Orientation::HORIZONTAL),
	prev_page("<-"),
	next_page("->"),
	current_page("loading...")
{
	current_page_index = 1;
	std::thread load_file_t([this](){
		load_file("./doc.pdf");

		Glib::signal_idle().connect_once([this]() {
        	build_pages_ui();   // GTK SAFE
   		 });

	});
	load_file_t.detach();

	// top panel
	top_panel.set_size_request(-1, 20);
	current_path_label.set_hexpand(true);
	back_button.set_size_request(35, -1);

	top_panel.append(back_button);
	top_panel.append(current_path_label);
	append(top_panel);
	// pages
	//Gtk::DrawingArea da_b();
	//pages.insert(0, std::move(da_b));

	
	pages_container.set_expand(true);
	

	pages_container.append(test);
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

void reader_component::build_pages_ui() {
    for (int i = 0; i < page_pixmaps.size(); i++) {

        auto da = Gtk::make_managed<Gtk::DrawingArea>();

		//da->set_expand(true);

        da->set_content_width(
			pages_container.get_width()
        );
        da->set_content_height(
			pages_container.get_height()
        );

        da->set_draw_func(
            [this, i](const Cairo::RefPtr<Cairo::Context>& cr, int width, int height){
                Gdk::Cairo::set_source_pixbuf(cr, page_pixmaps[i]);
                cr->paint();
            }
        );

        if (i != 3)
            da->hide();

        pages_container.append(*da);
        //da->show();
    }

    pages_container.show();
}

/*
void reader_component::build_pages_ui() {
		for (int i = 0; i < page_pixmaps.size(); i++) {
			Gtk::DrawingArea da = Gtk::DrawingArea();
			if (i != 2) da.hide();

			da.set_draw_func([this, i](const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
        			//if (page_pixmaps.empty())
    				//	return;
					
					Gdk::Cairo::set_source_pixbuf(cr, page_pixmaps[1]);
        			
					cr->paint();

    		});

			pages_container.append(da);

			//std::cout << "bbbbbbbb\n";
		};

		Gtk::Widget* child = pages_container.get_first_child();
		int count = 0;
		while (child != nullptr) {
    		count++;
    		child = child->get_next_sibling();
		}
		std::cout << count << "\n";

}
*/

/*
void reader_component::draw_page(const Cairo::RefPtr<Cairo::Context>& cr, Glib::RefPtr<Gdk::Pixbuf> buff, int width, int height) {
		Gdk::Cairo::set_source_pixbuf(cr, buff);
        cr->paint();
}
*/
