#include "./page-data.hpp"
#include "./reader.hpp"

#include <QtWidgets>
#include <QtCore>
#include <iostream>
#include <future>
#include <qtmetamacros.h>

page_data::page_data(thread_data * Data) {
	/*
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
	*/

	if (!Data->rendered) {
		render(Data);	
	}

	if (data->failed) {
		std::cout << "page_data::page_data(): " 
			  << "page at (" << Data->failed << ") is failed"
			  << "\n";
		return;
	};
	
	//data = Data;

	/*	
	QImage img(
		data->pix->samples,
		data->pix->w,
		data->pix->h,
		data->pix->stride,
		(data->pix->alpha) ? QImage::Format_RGBA8888 : QImage::Format_RGB888
	);
	w_pix = new QPixmap();
	*w_pix = QPixmap::fromImage(img);
	*/
	//fz_drop_pixmap(data->ctx, data->pix);
	//fz_drop_context(data->ctx);
};
page_data::page_data(thread_data * Data, void(* on_rendered)(thread_data *) ) {

	QObject::connect( this, &page_data::rendered,
        [on_rendered](thread_data* a) {
            if (on_rendered)
                on_rendered(a);
        });
	
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
page_data::~page_data() {
	fz_drop_pixmap(data->ctx, data->pix);
	fz_drop_display_list(data->ctx, data->list);
	
	fz_drop_context(data->ctx);

	
	free(data);
}

void page_data::render(thread_data * Data) {
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
	fz_try(ctx)
	{
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
		dat->failed = 1;
	}

	// Free this thread's context.
	fz_drop_context(ctx);
	
	//data->rendered = true;	
	data = dat;
	emit rendered(data);

	fprintf(stderr, "thread at page %d done!\n", pagenumber);
}

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
