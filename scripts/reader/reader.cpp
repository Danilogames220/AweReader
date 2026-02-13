#include "./reader.hpp"
#include "../global-variables.hpp"

#include <QtWidgets>
#include "mupdf/fitz.h"

#include <filesystem>
#include <iostream>
#include <qlabel.h>
#include <string>
#include <memory>
#include <thread>
#include <format>


void reader_component::create_page(page_data data) {
	QLabel * l = new QLabel(&pages_container);
	l->setPixmap(*data.w_pix);
	
	l->show();

	std::cout << "page: " << data.page_number << " done\n";
}

reader_component::reader_component(QWidget * parent) :
	QWidget(parent),
	reader_c_layout(this),

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




	// load file	
	
	QThread * t = QThread::create( [this](void) -> void{
		load_file(main_dir.filePath("doc.pdf").toStdString());
	});
	QObject::connect(this, &reader_component::page_rendered, this, &reader_component::create_page);
	t->start();


	// load ui
	reader_c_layout.setContentsMargins(0, 0, 0, 0);
	reader_c_layout.addWidget(&top_panel);
	reader_c_layout.addWidget(&pages_container);
	reader_c_layout.addWidget(&bottom_buttons);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

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
