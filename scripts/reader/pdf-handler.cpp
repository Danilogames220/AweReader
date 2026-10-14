#include <mupdf/fitz.h>
#include <mupdf/fitz/context.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "./reader.hpp"
#include "./pdf-handler.hpp"

void lock_mutex(void *user, int lock);
void unlock_mutex(void *user, int lock);
void fail(const char * msg);

pdf_handler::pdf_handler(const char * doc_name) {
	page_count = 0;
	doc = NULL;
	
	//pthread_t *thread = NULL;
	fz_locks_context locks;

	// Initialize FZ_LOCK_MAX number of non-recursive mutexes.
	int i;
	for (i = 0; i < FZ_LOCK_MAX; i++) {
		if (pthread_mutex_init(&mutex[i], NULL) != 0)
			fail("pthread_mutex_init()");
	}
	
	locks.user = mutex;
	locks.lock = lock_mutex;
	locks.unlock = unlock_mutex;

	ctx = fz_new_context(NULL, &locks, FZ_STORE_UNLIMITED);

	//fz_var(thread);
	fz_var(doc);

	fz_try(ctx)
	{
		fz_register_document_handlers(ctx);

		doc = fz_open_document(ctx, doc_name);

		page_count = fz_count_pages(ctx, doc);
		printf("total document pages: %d\n", page_count);
	}
	fz_catch(ctx)
	{
		fz_report_error(ctx);
		fz_drop_document(ctx, doc);
		fail("error");
	}
	
	threads = (pthread_t*)malloc(page_count*sizeof(pthread_t));
}
pdf_handler::~pdf_handler() {
	free(threads);
	fz_drop_document(ctx, doc);
	fz_drop_context(ctx);
	fflush(NULL);
}


void fail(const char *msg) {
	fprintf(stderr, "Mupdf Fail: %s\n", msg);
	abort();
}
void * pdf_handler::renderer(void *data_) {
	struct thread_data *data = (struct thread_data *)data_;
	int pagenumber = data->pagenumber;
	fz_context *ctx_t = data->ctx;
	fz_display_list *list = data->list;
	fz_rect bbox = data->bbox;
	fz_device *dev = NULL;
	
	fprintf(stderr, "thread at page %d loading!\n", pagenumber);

	ctx_t = fz_clone_context(ctx_t);

	fz_var(dev);

	fprintf(stderr, "thread at page %d rendering!\n", pagenumber);
	fz_try(ctx_t)
	{
		// Create a white pixmap using the correct dimensions. 
		data->pix = fz_new_pixmap_with_bbox(ctx_t, fz_device_rgb(ctx_t), fz_round_rect(bbox), NULL, 0); fz_clear_pixmap_with_value(ctx_t, data->pix, 0xff);

		// Do the actual rendering.
		dev = fz_new_draw_device(ctx_t, fz_identity, data->pix);
		float factor = data->matrix_factor;
		fz_run_display_list(ctx_t, list, dev, (fz_matrix){factor, 0, 0, factor, 0, 0}, bbox, NULL);
		fz_close_device(ctx_t, dev);
	}
	fz_always(ctx_t)
		fz_drop_device(ctx_t, dev);
	fz_catch(ctx_t)
		data->failed = 1;

	// Free this thread's context.
	fz_drop_context(ctx_t);

	fprintf(stderr, "thread at page %d done!\n", pagenumber);

	return data;
}

// cant put these in the class
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

struct thread_data * pdf_handler::get_data(pdf_handler &self, int index) {
	fz_context * ctx_t = fz_clone_context(self.ctx);
	fz_document * doc = self.doc;

	fz_page *page;
	fz_rect bbox;
	fz_display_list *list;
	fz_device *dev = NULL;
	//fz_pixmap *pix;
	struct thread_data *data;

	fz_var(dev);

	fz_try(ctx_t)
	{
		page = fz_load_page(ctx_t, doc, index);
		bbox = fz_bound_page(ctx_t, page);
		list = fz_new_display_list(ctx_t, bbox);
		dev = fz_new_list_device(ctx_t, list);
		fz_run_page(ctx_t, page, dev, fz_identity, NULL);
		fz_close_device(ctx_t, dev);
	}
	fz_always(ctx_t)
	{
		fz_drop_device(ctx_t, dev);
		fz_drop_page(ctx_t, page);
	}
	fz_catch(ctx_t)
		fz_rethrow(ctx_t);
	data = (struct thread_data*)malloc(sizeof (*data));

	data->pagenumber = index;
	data->ctx = ctx_t;
	data->list = list;
	data->bbox = bbox;
	data->pix = NULL;
	data->failed = 0;
	
	return data;
}

// TODO double-check this
page_data * pdf_handler::get_pixmap(int index, QSize space) {
	page_data * p_dat = new page_data();
	pthread_t thread;
	
	// do the zoom thing before rendering
	struct thread_data * data = get_data(*this, index);
	fz_context *ctx_t = data->ctx;
	
	// get zoom factor;
	float factor;
	int by = data->bbox.y1;
	int bx = data->bbox.x1;
	if (space.height() < space.width()) {
		//puts("factor height");
		factor = space.height()/(float)by;
	} else {
		//puts("factor width");
		factor = space.width()/(float)bx;
	}
	printf("factor of page %d: %f\n", data->pagenumber, factor);

	fz_matrix mult_matrix = (fz_matrix){factor, 0, 0, factor, 0, 0};
	data->bbox = fz_transform_rect(data->bbox, mult_matrix);
	data->matrix_factor = factor;
	
	struct thread_data * dat = (struct thread_data *)renderer((void *)data);	
	
	/*
	if (pthread_create(&thread, NULL, [](void * _data)-> void* {
		struct thread_data * dat = (struct thread_data *)renderer(_data);
		// do something after it finishes
		// (cant do much)
		return (void*)dat;
	}, data) != 0)
		fail("pthread_create()");
	if (pthread_join(thread, (void **)&dat) != 0)
		fail("pthread_join");
	if (dat->failed) 
	{
		//fprintf(stderr, "\tRendering for page %d failed\n", index + 1);
		printf("failed to get pixmap of page %d\n", index);
	}
	*/


	p_dat->index=index;

	QImage *label_img = new QImage(
		data->pix->samples,
		data->pix->w,
		data->pix->h,
		data->pix->stride,
		(data->pix->alpha) ? QImage::Format_RGBA8888 : QImage::Format_RGB888
	);
	
	p_dat->pixmap = new QPixmap();
	*p_dat->pixmap = QPixmap::fromImage(*label_img);

	
	// free mupdf stuff
	fz_drop_pixmap(ctx_t, dat->pix);
	fz_drop_display_list(ctx_t, dat->list);
	fz_drop_context(ctx_t);

	free(data);
	
	emit page_rendered(p_dat);
	return p_dat;
}

page_data ** pdf_handler::get_all_pixmap(QSize space) { 
	page_data ** data_v = (page_data**)malloc(page_count*sizeof(page_data*));
	for (int i = 0; i < page_count; i++)
		data_v[i] = new page_data();
	//pthread_t thread;
	
	// do the zoom thing before rendering
	
	for (int i = 0; i < page_count; i++) {
		struct thread_data * data = get_data(*this, i);
		fz_context *ctx_t = data->ctx;
		
		// get zoom factor;
		float factor;
		int by = data->bbox.y1;
		int bx = data->bbox.x1;
		if (space.height() < space.width()) {
			//puts("factor height");
			factor = space.height()/(float)by;
		} else {
			//puts("factor width");
			factor = space.width()/(float)bx;
		}
		printf("factor of page %d: %f\n", data->pagenumber, factor);
        
		fz_matrix mult_matrix = (fz_matrix){factor, 0, 0, factor, 0, 0};
		data->bbox = fz_transform_rect(data->bbox, mult_matrix);
		data->matrix_factor = factor;
		
		if (pthread_create(&threads[i], NULL, renderer, data) != 0)
			fail("pthread_create");
		//struct thread_data * dat = (struct thread_data *)renderer((void *)data);	
	}
	for (int i = 0; i < page_count; i++) {
		struct thread_data * dat;
		
		if (pthread_join(threads[i], (void **)&dat) != 0)
			fail("pthread_join");
		else
		{
			//dat = dv_t[i];
			/*
			char buff[16];
			sprintf(buff, "img%04d.png", i+1);

			fprintf(stderr, "\tSaving %s...\n", buff);
			fz_save_pixmap_as_png(ctx, dat->pix, buff);
			*/
			page_data * p_dat = data_v[i];

			//printf("i: %d\n", dat->pagenumber);
			//thread_data * data = data_v[i]->data;
			p_dat->index=i;
                
			QImage *label_img = new QImage(
				dat->pix->samples,
				dat->pix->w,
				dat->pix->h,
				dat->pix->stride,
				(dat->pix->alpha) ? QImage::Format_RGBA8888 : QImage::Format_RGB888
			);
			
			p_dat->pixmap = new QPixmap();
			*p_dat->pixmap = QPixmap::fromImage(*label_img);
			emit page_rendered(p_dat);


		}
		
		fz_context *ctx_t = dat->ctx;
		fz_drop_pixmap(ctx_t, dat->pix);
		fz_drop_display_list(ctx_t, dat->list);
		fz_drop_context(ctx_t);
		
		//free(dat);
	}
	

	return data_v;
}
