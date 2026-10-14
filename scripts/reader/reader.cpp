#include <QtCore>
#include <QtWidgets>

#include <iostream>
#include <string>

#include "./reader.hpp"
#include "./pdf-handler.hpp"

#include "../global-variables.hpp"

page_data::page_data() 
{
	zoom_factor = 1;
	position = QPointF(0,0);
	index = -1;

	pixmap_thread = nullptr;
}

// TODO: update this mess
void reader_component::init() {
	// start variables before loading the file 
	current_page_index = 0;
	can_resize = 0;
	
	// start pages with empty pointers
	//pages = std::vector<page_data*>(handler.page_count, nullptr);
	pages = (page_data**)malloc((handler.page_count)*sizeof(page_data*));
	for (int i = 0; i < handler.page_count; i++)
		pages[i] = nullptr;

	// load pages
	// TODO get this like it was before
	QObject::connect(&handler, &pdf_handler::page_rendered, this, &reader_component::add_page_to_reader);
	///*
	QThread * t = QThread::create( [this](void) -> void{
		handler.get_all_pixmap(pages_container.size());
		/*
		for (int i = 0; i < handler.page_count; i++) {
			handler.get_pixmap(i, pages_container.size());
		}
		*/
	});

	///*
	QObject::connect(t, &QThread::finished, this, [this]{
		// NOTE: pages.size() is from the vector
		current_page.setText(QString::fromStdString(std::format("{} / {}", current_page_index + 1, handler.page_count)));
		//puts("connecting signals");
		
		QObject::connect(&prev_button, &QPushButton::clicked,
			this, &reader_component::set_prev_page);

		QObject::connect(&next_button, &QPushButton::clicked,
			this, &reader_component::set_next_page);

		QObject::connect(&page_viewer, &ImageViewer::zoom_factor,
			this, &reader_component::zoom_page);

	});
	//*/
	t->start();
}


reader_component::reader_component() :
	QWidget(),
	handler(file_path.c_str()),
	reader_c_layout(this),
	
	// top panel
	top_panel(),
	top_layout(&top_panel),

	back_button("<"),
	current_path_label(QString::fromStdString(file_path)),

	// pages
	pages_container(),
	pages_layout(&pages_container),
	page_viewer(),

	// options
	bottom_buttons(),
	bb_layout(&bottom_buttons),

	prev_button("<-"),
	next_button("->"),
	current_page("Loading...")
	//current_page(QString::fromStdString(std::format("{} / {}", current_page_index + 1, handler.page_count)))
		
{
	// load ui
	reader_c_layout.setContentsMargins(0, 0, 0, 0);
	setLayout(&reader_c_layout);
	reader_c_layout.addWidget(&top_panel);
	reader_c_layout.addWidget(&pages_container);
	reader_c_layout.addWidget(&bottom_buttons);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	// bind widgetShow signal
	QObject::connect(this, &reader_component::widgetShow,
		this, &reader_component::init);

	// top panel
	top_panel.show();

	back_button.setFixedSize(30, 30);
		
	top_layout.addWidget(&back_button);
	top_layout.addWidget(&current_path_label);

	// pages
	pages_container.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	pages_container.show();
	pages_layout.setContentsMargins(0, 0, 0, 0);
	pages_layout.addWidget(&page_viewer);

	// bottom options
	bb_layout.setContentsMargins(0, 0, 0, 0);
	bottom_buttons.show();

	current_page.setAlignment(Qt::AlignCenter);

	bb_layout.addWidget(&prev_button);
	bb_layout.addWidget(&current_page);
	bb_layout.addWidget(&next_button);
};
reader_component::~reader_component() {
	free(pages);
}
// useful when testing signals
void reader_component::test_print() {
	puts("test print from reader_component");
}

void reader_component::set_page(int index) {
	if (index < 0 || index >= handler.page_count)
		return;
	
	//puts("reader set_page called");

	// update current page
	// nullptr check wont work and causes a segmentation fault
	if (pages[current_page_index] != nullptr && 
	    pages[index] != nullptr) {
		//container_scroll.takeWidget();
		//page_viewer.takePixmap();
		current_page_index = index;
		page_viewer.setPage(pages[index]);
	}

	// update page index text 
	current_page.setText(QString::fromStdString(
		std::format("{} / {}", current_page_index + 1, handler.page_count)
	));

		
};
void reader_component::set_next_page() {
	set_page(current_page_index + 1);
};
void reader_component::set_prev_page() {
	set_page(current_page_index - 1);
};


void reader_component::showEvent(QShowEvent * event) {
	QWidget::showEvent(event);
	emit widgetShow();
}

void reader_component::zoom_page(float zoom_factor) {

	pages[current_page_index]->pixmap_thread = QThread::create([this, zoom_factor]() {
		handler.get_pixmap(
			current_page_index, 
			pages_container.size()*zoom_factor
		);
	});

	pages[current_page_index]->pixmap_thread->start();
	//pages[current_page_index]->pixmap_thread->wait();
	
	
	//printf("zoom factor: %.2f\n", zoom_factor);
}

void reader_component::add_page_to_reader(page_data * page) {
	int p_index = page->index;
	
	if (pages[p_index] == nullptr) {
		pages[p_index] = page;
		
		// set this to be the current page for the viewer
		if (p_index == current_page_index)
			page_viewer.setPage(page);
	} else {
		pages[p_index]->pixmap = page->pixmap;
		
		page->pixmap_thread = nullptr;
		// update pixmap if this is the one being shown
		if (p_index == current_page_index)
			page_viewer.setPixmap(*page->pixmap);
	}
	
}
