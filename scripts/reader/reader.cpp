#include "./reader.hpp"
#include "../global-variables.hpp"

#include <QtWidgets>
#include "mupdf/fitz.h"

#include <iostream>
#include <string>
#include <thread>
#include <format>


void reader_component::showEvent(QShowEvent * event) {
	QWidget::showEvent(event);	


	// start variables before loading the file
	//set_page(0);
	current_page_index = 0;
	can_resize = 0;

	QObject::connect(this, &reader_component::page_rendered, this, &reader_component::add_page_to_reader);
	
	// load file	
	QThread * t = QThread::create( [this](void) -> void{
		//load_file(main_dir.filePath("doc.pdf").toStdString());
		load_file(file_path);
	});
	//QObject::connect(this, &reader_component::page_rendered, this, &reader_component::create_page);
	
	// after load_file finished
	QObject::connect(t, &QThread::finished, this, [this]{
		std::cout << "pages size: " << pages.size() << "\n";
		

		current_page.setText(QString::fromStdString(std::format("{} / {}", current_page_index + 1, page_count)));
		QObject::connect(&prev_button, &QPushButton::clicked,
				this, &reader_component::set_prev_page);

		QObject::connect(&next_button, &QPushButton::clicked,
			this, &reader_component::set_next_page);

	});
	t->start();
}
void reader_component::resizeEvent(QResizeEvent * event) {
	QWidget::resizeEvent(event);
	
	// resize current page in case of window resizing
	// segmentation fault
	//if (can_resize) pages[current_page_index]->resize(pages_container.geometry());
	
	std::cout << pages_container.height() << "\n";
}

// might be better to start variables inside showEvent than heve
reader_component::reader_component() :
	QWidget(),
	reader_c_layout(this),

	// top panel
	top_panel(),
	top_layout(&top_panel),


	back_button("<"),
	current_path_label(QString::fromStdString(file_path)),

	// pages
	pages_container(),

	// options
	bottom_buttons(),
	bb_layout(&bottom_buttons),

	prev_button("<-"),
	next_button("->"),
	current_page(QString::fromStdString(std::format("{} / {}", current_page_index + 1, page_count)))
		
{
	// load ui
	reader_c_layout.setContentsMargins(0, 0, 0, 0);
	setLayout(&reader_c_layout);
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


void reader_component::set_page(int index) {
	if (index >= page_count || index < 0) {
		return;
	};
	if (!pages[current_page_index]->label) {
		return;
	}
		
	// update current page
	// nullptr check wont work and causes a segmentation fault
	if (pages[current_page_index] != nullptr && 
	    pages[index] != nullptr) {
		pages[current_page_index]->label->hide();
		current_page_index = index;
		pages[current_page_index]->label->show();
	}

	// update page count thing
	current_page.setText(QString::fromStdString(
		std::format("{} / {}", current_page_index + 1, page_count)
	));

		
};

void reader_component::set_next_page() {
	set_page(current_page_index + 1);
};
void reader_component::set_prev_page() {
	set_page(current_page_index - 1);
};
