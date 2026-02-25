// code "borrowed" from example:
// https://mupdf.readthedocs.io/en/1.27.0/cookbook/c/multi-threaded.html

// TODO
// - zoom page and resize acording to pages_container size;
// 
// - way to start rendering from any page

#include "./reader.hpp"
#include "page-data.hpp"

#include <mupdf/fitz.h>
#include <QtWidgets>

#include <stdlib.h>
#include <pthread.h> // c++ mutex wont work with mupdf
#include <string>
#include <iostream>
#include <thread>
#include <future>


void fail(const char *msg) {
	fprintf(stderr, "%s\n", msg);
	abort();
}

void lock_mutex(void *user, int lock) {
	pthread_mutex_t *mutex = (pthread_mutex_t *) user;

	if (pthread_mutex_lock(&mutex[lock]) != 0)
		fail("pthread_mutex_lock()");
}
void unlock_mutex(void *user, int lock) {
	pthread_mutex_t *mutex = (pthread_mutex_t *) user;

	if (pthread_mutex_unlock(&mutex[lock]) != 0)
		fail("pthread_mutex_unlock()");
}

void reader_component::load_file(std::string path) {
	fz_context * ctx;
	fz_document * doc = NULL;
	fz_locks_context locks;

	std::vector<std::future<page_data *>> thread;

	pthread_mutex_t mtx[FZ_LOCK_MAX];	

	// Initialize FZ_LOCK_MAX number of non-recursive mutexes.
	for (int i = 0; i < FZ_LOCK_MAX; i++)
	{
		if (pthread_mutex_init(&mtx[i], NULL) != 0)
			fail("pthread_mutex_init()");
	};
	
	// i think this only works with pthread_mutex_t
	locks.user = mtx;
	locks.lock = lock_mutex;
	locks.unlock = unlock_mutex;
	
	ctx = fz_new_context(NULL, &locks, FZ_STORE_UNLIMITED);

	fz_var(doc);

	fz_try(ctx) {
		// Register default file types.
		fz_register_document_handlers(ctx);

		// Open the PDF, XPS or CBZ document.
		doc = fz_open_document(ctx, path.c_str());

		page_count = fz_count_pages(ctx, doc);
	
		pages.clear();
		pages.resize(page_count);
		
		for (int i = 0; i < page_count; i++) {
			fz_page *page;
			fz_rect bbox;
			fz_display_list *list;
			fz_device *dev = NULL;
			fz_pixmap *pix;

			struct thread_data *data;

			fz_var(dev);

			fz_try(ctx) {
				page = fz_load_page(ctx, doc, i);
				bbox = fz_bound_page(ctx, page);
				list = fz_new_display_list(ctx, bbox);
				dev = fz_new_list_device(ctx, list);
				fz_run_page(ctx, page, dev, fz_identity, NULL);
				fz_close_device(ctx, dev);
			}
			fz_always(ctx) {
				
				fz_drop_device(ctx, dev);

				fz_drop_page(ctx, page);
			}
			fz_catch(ctx) {
				fz_rethrow(ctx);
			}

			data = (thread_data *)malloc(sizeof (*data));

			data->pagenumber = i + 1;
			data->ctx = ctx;
			data->list = list;
			data->bbox = bbox;
			data->pix = NULL;
			data->failed = 0;

			thread.insert(
				thread.begin() + i, 
				std::async(std::launch::async, [this, data]() -> page_data* {
				page_data * buf = new page_data(
					QSize(0, 0), 
					data, 
					[this, buf](page_data * d)-> void {
						emit this->page_rendered(d);
					} 
				);
				
				return buf;
			}));
		};
	}
	fz_always(ctx) {
		thread.clear();

		// Drop the document
		fz_drop_document(ctx, doc);
	}
	fz_catch(ctx) {
		fz_report_error(ctx);
		fail("error");	
	};
	
	printf("file loaded succefully\n");
	//clear
	fz_drop_context(ctx);
}
