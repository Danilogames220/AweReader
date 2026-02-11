#include "./reader.hpp"
#include "../global-variables.hpp"

#include <QtWidgets>
#include "mupdf/fitz.h"

#include <filesystem>
#include <iostream>
#include <qcoreapplication.h>
#include <string>
#include <memory>
#include <thread>
#include <format>

reader_component::reader_component(QWidget * parent) :
	QVBoxLayout(parent),
	//reader_c_layout(this),

	// top panel
	top_panel(),
	top_layout(&top_panel),

	back_button("<"),
	current_path_label("~/books/stuff/doc.pdf"),

	// pages
	pages_container(),

	// options
	bottom_buttons(),
	bb_layout(&bottom_buttons),

	prev_button("<-"),
	next_button("->"),
	current_page("loading...")
{
	current_page_index = 0;
	
	load_file(main_dir.filePath("doc.pdf").toStdString());

	//std::cout << main_dir.filePath("doc.pdf").toStdString() << "\n";

	/*
	// load file
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
	*/
	// remove later
	QImage img(
		pages[0].pix->samples,
		pages[0].pix->w,
		pages[0].pix->h,
		pages[0].pix->stride,
		QImage::Format_RGB888
	);
	QLabel * test = new QLabel(&pages_container);
	test->setPixmap(QPixmap::fromImage(img));
	test->show();

	// load ui
	setContentsMargins(0, 0, 0, 0);
	addWidget(&top_panel);
	addWidget(&pages_container);
	addWidget(&bottom_buttons);

	// top panel
	top_panel.show();

	back_button.setFixedSize(30, 30);
	
	top_layout.addWidget(&back_button);
	top_layout.addWidget(&current_path_label);

	// pages
	pages_container.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	pages_container.show();

	// bottom options
	bb_layout.setContentsMargins(0, 0, 0, 0);
	bottom_buttons.show();

	current_page.setAlignment(Qt::AlignCenter);

	bb_layout.addWidget(&prev_button);
	bb_layout.addWidget(&current_page);
	bb_layout.addWidget(&next_button);
};

/*
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

*/
