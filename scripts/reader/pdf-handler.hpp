#ifndef PDF_HANDLER_HPP
#define PDF_HANDLER_HPP

#include <mupdf/fitz.h>
#include <mupdf/fitz/context.h>
#include <stdlib.h>
#include <pthread.h>
#include <QtCore>
#include <QtWidgets>
#include <vector>

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


struct pix_request {
	page_data * p_data;
	pthread_t thread;
	char ongoing;
};


class pdf_handler : public QObject {
	Q_OBJECT

	public:
		unsigned int page_count;

		fz_context *ctx;
		fz_document *doc;

		pdf_handler(const char * doc_name);
		~pdf_handler();
		
		page_data * get_pixmap(int index, QSize space);

	private:
		//where all render threads go
		std::vector<pix_request> pix_queries;

		pthread_mutex_t mutex[FZ_LOCK_MAX];
		static void * renderer(void *data_);
		static struct thread_data * get_data(pdf_handler &self, int index);
	
	//public slots:
	//	void new_zoom_query(float factor);

	signals:
		void page_rendered(page_data * page);
};

#endif
