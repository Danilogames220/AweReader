#ifndef PAGE_DATA_HPP
#define PAGE_DATA_HPP

#include <QtCore>
#include <QtWidgets>
#include <functional>
#include <mupdf/fitz.h>

struct thread_data {
	bool rendered;

	fz_context *ctx;
	int pagenumber;
	fz_display_list *list;
	fz_rect bbox;
	fz_pixmap *pix;
	int failed;
};

// page_data cant be moved, like qt widgets
class page_data : public QObject {
	Q_OBJECT

	public:
		page_data(thread_data * Data);
		page_data(thread_data * Data, void(* on_rendered)(thread_data *));

		~page_data();

		thread_data * data;

		// qt stuff
		QLabel * label;
		QPixmap * w_pix;

		// gui stuff
		bool is_centered;
		float zoom;


		void resize(QRect rect);

		// make it work like render_pages_thread()
		void render(thread_data * Data);
		// loads the current data to widgets
		void load();
	
	signals:
		void rendered(thread_data * Data);

};

#endif // !PAGE_DATA_HPP
