// code "borrowed" from example:
// https://mupdf.readthedocs.io/en/1.27.0/cookbook/c/multi-threaded.html

// TODO
// - render each page individualy on page_data instead of render_page_thread
// 	- render with the data passed in the constuctor
// 	- zoom page and resize acording to container size;
// 
// - way to start rendering from any page

#include "./reader.hpp"

#include <mupdf/fitz.h>
#include <QtWidgets>

#include <mupdf/fitz/color.h>
#include <mupdf/fitz/context.h>
#include <mupdf/fitz/display-list.h>
#include <qpixmap.h>
#include <stdlib.h>
#include <pthread.h> // c++ mutex wont work with mupdf
#include <string>
#include <iostream>
#include <thread>
#include <future>

// page_data stuff
page_data::page_data(fz_context *Ctx, int Pagenumber, fz_display_list *List, fz_rect Bbox, fz_pixmap * Pixmap, int Failed) {
	// load values
	ctx = Ctx;
	ctx = fz_clone_context(ctx);
	page_number = Pagenumber;
	list = List;
	//memcpy(list, List, sizeof(fz_display_list);
	bbox = Bbox;
	pix = Pixmap; // qt already clones pixmaps
	//memcpy(pix, Pixmap, sizeof(fz_pixmap));

	failed = Failed;

	// load qt stuff
	if (failed) return;

	QImage img(
		pix->samples,
		pix->w,
		pix->h,
		pix->stride,
		(pix->alpha) ? QImage::Format_RGBA8888 : QImage::Format_RGB888
	);
	w_pix = new QPixmap();
	*w_pix = QPixmap::fromImage(img);
	
	fz_drop_pixmap(ctx, pix);
	fz_drop_context(ctx);
};

void page_data::resize(QRect rect) {
	if (!w_pix) return;

	*w_pix = w_pix->scaledToWidth(rect.width());
	*w_pix = w_pix->scaledToHeight(rect.height());
	/*
	if (w_pix->height() > w_pix->width()) {
		*w_pix = w_pix->scaledToWidth(size.width());
		*w_pix = w_pix->scaledToHeight(size.height());
	} else {
		*w_pix = w_pix->scaledToWidth(size.width());
		*w_pix = w_pix->scaledToHeight(size.height());
	}
	*/
	if (label) {
        	label->setPixmap(*w_pix);
        	label->resize(w_pix->size());

		label->move((rect.width() - label->width())/2 + rect.x(),
			(rect.height() - label->height())/2 + rect.y());
    	}

	//if (label) label->setFixedSize(w_pix->size());


}

// load_file() stuff

struct thread_data {
	fz_context *ctx;
	int pagenumber;
	fz_display_list *list;
	fz_rect bbox;
	fz_pixmap *pix;
	int failed;
};

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

	std::vector<std::future<void *>> thread;

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

			// Create the thread and pass it the data structure.
			thread.insert(thread.begin() + i, std::async(std::launch::async, &reader_component::page_render_thread, this, data));
		}
		

		fprintf(stderr, "joining %d threads...\n", page_count);
		for (int i = 0; i < page_count; i++) {
			struct thread_data *data;
		
			data = (thread_data *)thread[i].get();

			if (data->failed)
			{
				fprintf(stderr, "\tRendering for page %d failed\n", i + 1);
			}
			else
			{
				//fprintf(stderr, "\tSaving %s...\n", filename);

				// Write the rendered image to a PNG file
				//fz_save_pixmap_as_png(ctx, data->pix, filename);
	
				//page_data buff(data->ctx, data->pagenumber, data->list, data->bbox, data->pix, data->failed);
				//buff.load_label(&pages_container);
				//pages.insert(pages.begin() + i, std::move(buff));

			}
			fz_drop_display_list(ctx, data->list);

			free(data);
		}

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

void * reader_component::page_render_thread(void *data_) {

	struct thread_data *data = (struct thread_data *)data_;
	int pagenumber = data->pagenumber;
	fz_context *ctx = data->ctx;
	fz_display_list *list = data->list;
	fz_rect bbox = data->bbox;
	fz_device *dev = NULL;

	fprintf(stderr, "thread at page %d loading!\n", pagenumber);

	// The context pointer is pointing to the main thread's
	// context, so here we create a new context based on it for
	// use in this thread.
	ctx = fz_clone_context(ctx);

	// Next we run the display list through the draw device which
	// will render the request area of the page to the pixmap.

	fz_var(dev);

	fprintf(stderr, "thread at page %d rendering!\n", pagenumber);
	fz_try(ctx)
	{
		// Create a white pixmap using the correct dimensions.
		data->pix = fz_new_pixmap_with_bbox(ctx, fz_device_rgb(ctx), fz_round_rect(bbox), NULL, 0);
		fz_clear_pixmap_with_value(ctx, data->pix, 0xff);

		// Do the actual rendering.
		dev = fz_new_draw_device(ctx, fz_identity, data->pix);
		fz_run_display_list(ctx, list, dev, fz_identity, bbox, NULL);
		fz_close_device(ctx, dev);

	}
	fz_always(ctx)
		fz_drop_device(ctx, dev);
	fz_catch(ctx)
		data->failed = 1;

	// Free this thread's context.
	fz_drop_context(ctx);

	fprintf(stderr, "thread at page %d done!\n", pagenumber);
	
	page_data aa(data->ctx, data->pagenumber, data->list, data->bbox, data->pix, data->failed);

	emit page_rendered(std::move(aa));
	
	return data;
};
