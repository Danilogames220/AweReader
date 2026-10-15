#ifndef PDF_HANDLER_HPP
#define PDF_HANDLER_HPP

#include <mupdf/fitz.h>
#include <mupdf/fitz/context.h>
#include <stdlib.h>
#include <pthread.h>
#include <QtCore>
#include <QtWidgets>

#include "../global-variables.hpp"
//#include "./reader.hpp"

struct thread_data {
	fz_context *ctx;
	int pagenumber;
	fz_display_list *list;

	fz_rect bbox;
	fz_pixmap *pix;

	float matrix_factor;

	int failed;
};

class pdf_handler : public QObject {
	Q_OBJECT

	public:
		unsigned int page_count;

		fz_context *ctx;
		fz_document *doc;

		pdf_handler(const char * doc_name);
		~pdf_handler();
		
		// used for getting one single pixmap
		page_data * get_pixmap(int index, QSize space);
		// TODO: function that gets pages across an interval (all pages from x to y)
		// gets all pixmaps in the document
		// NOTE: since the reader will only open a single doc, this dosent need to return any way of a dynamic list 
		page_data ** get_all_pixmap(QSize space);
		
		void query_pixmap(page_data * page, QSize space);

	private:

		pthread_mutex_t mutex[FZ_LOCK_MAX];
		pthread_t *threads;

		static void * renderer(void *data_);
		static struct thread_data * get_data(pdf_handler &self, int index);

	signals:
		void page_rendered(page_data * page);
};

#endif
