#include "./reader.hpp"
#include "../global-variables.hpp"

#include <QtWidgets>
#include "mupdf/fitz.h"

#include <iostream>
#include <qnamespace.h>
#include <string>
#include <thread>
#include <format>


void reader_component::showEvent(QShowEvent * event) {
	QWidget::showEvent(event);	


	// start variables before loading the file
	current_page_index = 0;
	can_resize = 0;

	QObject::connect(this, &reader_component::page_rendered, this, &reader_component::add_page_to_reader);
	
	// load file	
	QThread * t = QThread::create( [this](void) -> void{
		load_file(pages_container.size(), file_path);
	});
	
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
}

void reader_component::do_showEvent(QKeyEvent * event) {
	keyPressEvent(event);
}
void reader_component::keyPressEvent(QKeyEvent * event) {
	puts("key pressed in reader");
}



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
