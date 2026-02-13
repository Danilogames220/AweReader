#ifndef READER_HPP
#define READER_HPP
#include <QtWidgets>
#include <mupdf/fitz.h>

#include <qtmetamacros.h>
#include <vector>

class page_data {
	public:
		page_data(fz_context *Ctx, int Pagenumber, fz_display_list *List, fz_rect Bbox, fz_pixmap * Pixmap, int Failed);
		//~page_data();

		// mupdf stuff
		fz_context *ctx;
		int page_number;
		fz_display_list *list;
		fz_rect bbox;
		fz_pixmap *pix;
		int failed;

		// qt stuff
		QLabel * label;
		QPixmap * w_pix;

		void load_label(QWidget * parent);

		// make it work like render_pages_thread()
		//void render();

};

class reader_component : public QWidget {
	Q_OBJECT

	protected:
		void showEvent(QShowEvent *event) override;
	// UI stuff
	public: 
		reader_component(QWidget * parent);
		//void showEvent(QShowEvent* event);

		QVBoxLayout reader_c_layout;

		// top panel
		QWidget top_panel;
		QHBoxLayout top_layout;
			QPushButton back_button;
			QLabel current_path_label;
		
		// where the pages will get displayed in the window
		QWidget pages_container;
			std::vector<page_data> pages;
		
		// bottom buttons
		QWidget bottom_buttons;
		QHBoxLayout bb_layout;
			QPushButton prev_button;
			QLabel current_page;
			QPushButton next_button;

	public slots:
		void create_page(page_data data);

	signals:
		void page_rendered(page_data page);	

	// stuff related to processing the file
	private: 
		uint64_t current_page_index;
		uint64_t page_count;

		void load_file(std::string path);
			
		// used to load each page by itself in a specific thread
		void * page_render_thread(void *data_);
		
		// ui related functions
		void on_pages_loaded();

		void set_page(int index);

		void set_next_page();
		void set_prev_page();
};


#endif // READER_HPP
