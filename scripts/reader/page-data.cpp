#include "./page-data.hpp"
#include "./reader.hpp"

#include <QtWidgets>
#include <QtCore>
#include <QImage>

#include <iostream>
#include <future>
#include <qpixmap.h>
#include <qtmetamacros.h>

/*
void page_data::resize(QRect rect) {
	if (!w_pix) return;

	*w_pix = w_pix->scaledToWidth(rect.width());
	*w_pix = w_pix->scaledToHeight(rect.height());
	if (label) {
        	label->setPixmap(*w_pix);
        	label->resize(w_pix->size());

		label->move((rect.width() - label->width())/2 + rect.x(),
			(rect.height() - label->height())/2 + rect.y());
    	}

	//if (label) label->setFixedSize(w_pix->size());

}
*/
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
void page_data::render(QSize size, thread_data * Data) {
	struct thread_data *dat = Data;
	int pagenumber = dat->pagenumber;
	fz_context *ctx = dat->ctx;
	fz_display_list *list = dat->list;
	fz_rect bbox = dat->bbox;
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
	fz_try(ctx) {
		// Create a white pixmap using the correct dimensions.
		dat->pix = fz_new_pixmap_with_bbox(ctx, fz_device_rgb(ctx), fz_round_rect(bbox), NULL, 0);
		fz_clear_pixmap_with_value(ctx, dat->pix, 0xff);

		// Do the actual rendering.
		dev = fz_new_draw_device(ctx, fz_identity, dat->pix);
		fz_run_display_list(ctx, list, dev, fz_identity, bbox, NULL);
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
page_data::page_data(QSize size, thread_data * Data, ON_RENDERED) {
	if (on_rendered != nullptr)
		QObject::connect( this, &page_data::rendered, this, on_rendered);
	
	render(size, Data);	
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
	page->label->setParent(&pages_container);
	
	//std::cout << "page (" << page->data->pagenumber - 1 << "added\n";

	if (page_index == current_page_index) {
		page->label->show();
	}
	pages[page_index] = page;
	puts("nnnn");
}

