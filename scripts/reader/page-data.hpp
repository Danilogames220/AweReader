#ifndef PAGE_DATA_HPP
#define PAGE_DATA_HPP

#include <QtCore>
#include <QtWidgets>
#include <functional>
#include <mupdf/fitz.h>

//#define ON_RENDERED void(* on_rendered)(page_data *)
#define ON_RENDERED std::function<void(page_data *)> on_rendered

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
		// copies Data
		page_data(thread_data * Data);
		// renders Data
		page_data(thread_data * Data, ON_RENDERED);
		page_data(QSize size, thread_data * Data, ON_RENDERED);
		~page_data();

		thread_data * data;

		QLabel * label;
		QPixmap * label_pix;
		QImage * label_img;

		QPoint position;
		bool is_centered;

		// zoom aplied from rendering with QSize
		float internal_zoom;
		// zoom from the reader itself
		float extenal_zoom;

		// --- METHODS --- //
		//void resize(QSize size);

		// re-render the page resizing acording to size, but in a seprate thread
		void query_resize(QSize size, ON_RENDERED);
		//void query_resize(QSize size);

		//void render(thread_data * Data);
		void render(QSize size, thread_data * Data);
		//void render(QSize size,thread_data * Data);
		
		// loads the current data to widgets
		void load_widget();
	
	signals:
		void rendered(page_data * self);

};

#endif // !PAGE_DATA_HPP
