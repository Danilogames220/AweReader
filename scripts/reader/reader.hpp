#ifndef READER_HPP
#define READER_HPP
#include <QtCore>
#include <QtWidgets>
#include <mupdf/fitz.h>

#include <vector>

#include "./page-data.hpp"

class reader_component : public QWidget {
	Q_OBJECT
	
	protected:
		// load_file() happens here
		void showEvent(QShowEvent * event) override;
		void resizeEvent(QResizeEvent * event) override;


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
			std::vector<page_data *> pages;
		
		// bottom buttons
		QWidget bottom_buttons;
		QHBoxLayout bb_layout;
			QPushButton prev_button;
			QLabel current_page;
			QPushButton next_button;

	public slots:
		void create_page(page_data * data);

		void set_next_page();
		void set_prev_page();
	
	signals:
		void page_rendered(page_data *  page);	
		

	// stuff related to processing the file
	private: 
		uint64_t current_page_index;
		uint64_t page_count;

		bool can_resize;

		void load_file(std::string path);
			
		// used to load each page by itself in a specific thread
		void * page_render_thread(void *data_);
		
		// ui related functions
		void on_pages_loaded();

		void set_page(int index);

};


#endif // READER_HPP
