#ifndef READER_HPP
#define READER_HPP
#include <QtCore>
#include <QtWidgets>
#include <mupdf/fitz.h>

#include <qscrollarea.h>

#include "../global-variables.hpp"
#include "./viewer.hpp"
#include "./pdf-handler.hpp"


class reader_component : public QWidget {
	Q_OBJECT
	
	protected:
		void showEvent(QShowEvent * event) override;
		//void resizeEvent(QResizeEvent * event) override;

	// UI stuff
	public: 
		reader_component();
		QVBoxLayout reader_c_layout;

		// top panel
		QWidget top_panel;
		QHBoxLayout top_layout;
			QPushButton back_button;
			QLabel current_path_label;
		
		// where the pages will get displayed in the window
		QWidget pages_container;
		QHBoxLayout pages_layout;
			SceneImageViewer page_viewer;
			//QScrollArea container_scroll;
			std::vector<page_data *> pages;
		
		// bottom buttons
		QWidget bottom_buttons;
		QHBoxLayout bb_layout;
			QPushButton prev_button;
			QLabel current_page;
			QPushButton next_button;

	// stuff related to processing the file
	private: 
		pdf_handler handler;

		uint64_t current_page_index;

		bool can_resize;

		void set_page(int index);
	public slots:
		void zoom_page(float zoom_factor);
		
		void add_page_to_reader(page_data * page);
		
		void set_next_page();
		void set_prev_page();
		
		void test_print();
		void init();
	signals:
		//void page_rendered(page_data * page);	
		void widgetShow();	
};

#endif // READER_HPP
