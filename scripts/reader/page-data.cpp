#include "./page-data.hpp"
#include "./reader.hpp"

#include <QtWidgets>
#include <QtCore>
#include <QImage>

#include <cmath>
#include <iostream>
#include <future>
#include <mupdf/fitz/geometry.h>


void page_data::load_widget() {
	if (data == nullptr) {
		std::cerr << "page_data::load_widget(): "
			  << "page.data pointer is null"
			  << "\n";
		return;
	};

	label_img = new QImage(
		data->pix->samples,
		data->pix->w,
		data->pix->h,
		data->pix->stride,
		(data->pix->alpha) ? QImage::Format_RGBA8888 : QImage::Format_RGB888
	);
	
	label_pix = new QPixmap();
	*label_pix = QPixmap::fromImage(*label_img);
	
	label = new QLabel();
	label->setPixmap(*label_pix);
	label->hide();

};
void page_data::render(fz_document * doc, QSize size, thread_data * Data) {
	QSizeF size_factor(0, 0);
	bool size_empty = true;

	struct thread_data *dat = Data;
	int pagenumber = dat->pagenumber;
	fz_context *ctx = dat->ctx;
	fz_display_list *list = dat->list;
	fz_rect bbox = dat->bbox;
	fz_device *dev = NULL;
	fz_matrix ctm = dat->ctm;

	printf("x0= %f\n", bbox.x0);
	printf("x1= %f\n", bbox.x1);
	printf("difrence between x0 and x1 = %.02f\n", bbox.x0 - bbox.x1);

	// check if size is empty
	if (size.width() > 0 && size.height() > 0) {
		//puts("size is not 0");
		size_empty = false;
	}

	fprintf(stderr, "thread at page %d loading!\n", pagenumber);

	// The context pointer is pointing to the main thread's
	// context, so here we create a new context based on it for
	// use in this thread.
	ctx = fz_clone_context(ctx);

	// Next we run the display list through the draw device which
	// will render the request area of the page to the pixmap.

	// resize page acording to size
	

	fz_var(dev);

	// where the rendering actualy happens
	fprintf(stderr, "thread at page %d rendering!\n", pagenumber);
	fz_try(ctx) {
		
		if (!size_empty) {
			//bbox = {.x0 = 0, .y0 = 0, .x1 = 100, .y1 = 100};
			size_factor = 2.84 * get_resize_factors(size, QSizeF(bbox.x1, bbox.y1));

			// works in doc.pdf
			//#define N / (1.5 + .125)
			// works in ball.pdf
			//#define N * (3.125 + 0.0075) 
			
			if (size_factor.width() > size_factor.height()) {
				fz_rect n = bbox;
				
				bbox.x1 *= 1/size_factor.width(); 
				bbox.y1 *= 1/size_factor.width();	

				//#define N * (3.125 + 0.0075) 
						    
				//ctm = fz_transform_page(n, 1/size_factor.height(), 0);
				ctm = fz_scale((n.x1 / bbox.x1), (n.y1 / bbox.y1));

				//dat->pix->w *= 1/size_factor.height();
					//bbox.x1 *= 1/size_factor.width();
					//bbox.y1 *= 1/size_factor.width();

					//dat->pix->w *= 1/size_factor.width();
					//dat->pix->h *= 1/size_factor.width();
			} else {
				puts("height is the biggest");

				fz_rect n = bbox;
				
				bbox.x1 *= 1/size_factor.height(); 
				bbox.y1 *= 1/size_factor.height();	

				//#define N / (1.5 + .125)
				//#define N * (3.125 + 0.0075) 
						    
				//ctm = fz_transform_page(n, 1/size_factor.height(), 0);
				ctm = fz_scale((n.x1 / bbox.x1), (n.y1 / bbox.y1));

				//dat->pix->w *= 1/size_factor.height();
				//dat->pix->h *= 1/size_factor.height();
			}; 
		}
		

		// Create a white pixmap using the correct dimensions.
		dat->pix = fz_new_pixmap_with_bbox(ctx, fz_device_rgb(ctx), fz_round_rect(bbox), NULL, 0);
		fz_clear_pixmap_with_value(ctx, dat->pix, 0xff);
	
		// Do the actual rendering.
		dev = fz_new_draw_device(ctx, ctm, dat->pix);
		fz_run_display_list(ctx, list, dev, ctm, bbox, NULL);
		
		fz_close_device(ctx, dev);
		
	}
	fz_always(ctx) {
		fz_drop_device(ctx, dev);
	}
	fz_catch(ctx) {
		fprintf(stderr, "thread at page %d failed!\n", pagenumber);
		dat->failed = 1;
	}

	// Free this thread's context.
	fz_drop_context(ctx);
	
	//data->rendered = true;	
	data = dat;
	emit rendered(this);

	fprintf(stderr, "thread at page %d done!\n", pagenumber);
	
}

//void page_data::render(thread_data * Data) {
//	std::cout << "render is empty\n";
//};

/*
page_data::page_data(thread_data * Data) {

	if (!Data->rendered) {
		render(Data);	
	}

	if (data->failed) {
		std::cout << "page_data::page_data(): " 
			  << "page at (" << Data->failed << ") is failed"
			  << "\n";
		return;
	};
	
};
*/

// NOTE on_rendered might trigger again in case of this object rendering again.
page_data::page_data(fz_document * doc, QSize size, thread_data * Data, ON_RENDERED) {
	if (on_rendered != nullptr)
		QObject::connect( this, &page_data::rendered, this, on_rendered);
	
	render(doc, size, Data);
	if (data->failed) {
		std::cout << "page_data::page_data() : render():  " 
			  << "page at (" << data->pagenumber << ") failed rendering"
			  << "\n";
		return;
	};
	//load_widget();

};
page_data::~page_data() {
	fz_drop_pixmap(data->ctx, data->pix);
	fz_drop_display_list(data->ctx, data->list);
	
	fz_drop_context(data->ctx);
	
	free(data);
}

// show page on the reader
void reader_component::add_page_to_reader(page_data * page) {
	int page_index = page->data->pagenumber - 1;

	page->load_widget();

	page->label->move(
		(pages_container.width() - page->label_pix->width())/2,
		(pages_container.height() - page->label_pix->height())/2
	);
	page->label->setParent(&pages_container);
	
	//std::cout << "page (" << page->data->pagenumber - 1 << "added\n";

	if (page_index == current_page_index) {
		page->label->show();
	}
	pages[page_index] = page;
}

