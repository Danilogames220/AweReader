#include "./reader.hpp"
#include "gdkmm/general.h"
#include "gdkmm/pixbuf.h"
#include "mupdf/fitz.h"

#include <cstdio>
#include <gtkmm.h>

#include <iostream>
#include <string>
#include <memory>


reader_component::reader_component() :
	Gtk::Box(Gtk::Orientation::VERTICAL),

	pages_container(),

	// top panel
	top_panel(Gtk::Orientation::HORIZONTAL),
	back_button("<"),
	current_path_label("~/books/stuff/doc.pdf"),

	// pages
	test(),

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
	
	test.set_expand(true);
	test.set_draw_func([this](const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
        if (page_pixmaps.empty())
    		return;

		fz_pixmap *pm = (fz_pixmap *)page_pixmaps[0];

		Glib::RefPtr<Gdk::Pixbuf> buff = Gdk::Pixbuf::create_from_data(
			pm->samples,
			Gdk::Colorspace::RGB,
    		pm->alpha,          // has alpha
    		8,             // bits per sample
    		pm->w,
    		pm->h,
    		pm->stride
		);
		// TODO free pixmap after loading it into gtk
		//fz_drop_pixmap(ctx, data->pix);


		//Gdk::Cairo::set_source_pixbuf(const int &context, const int &pixbuf)

		Gdk::Cairo::set_source_pixbuf(cr, buff);
        cr->paint();
    }
);
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
	
	load_file("./doc.pdf");

};

