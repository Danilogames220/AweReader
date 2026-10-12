#include <QtCore>
#include <QtWidgets>
#include <QGraphicsView>
#include "./viewer.hpp"

#include <iostream>
#include <qevent.h>
#include <qnamespace.h>

#define ZOOM_FACTOR 1.5

ImageViewer::ImageViewer() {
	current_zoom = 1.;
	current_page = nullptr;
};

void ImageViewer::paintEvent(QPaintEvent *) {
        QPainter p{this};

        p.translate(rect().center());
	p.scale(current_zoom, current_zoom);

	if (current_page != nullptr)
		p.translate(m_delta*current_page->zoom_factor);
	//p.translate(m_delta*current_zoom);
	p.drawPixmap(m_rect.topLeft(), m_pixmap);
}
void ImageViewer::mousePressEvent(QMouseEvent *event) {
        m_reference = event->pos();
        qApp->setOverrideCursor(Qt::ClosedHandCursor);
        setMouseTracking(true);
}
void ImageViewer::mouseMoveEvent(QMouseEvent *event) {
	//current_page->position += (event->pos() - m_reference) * 1.0/current_zoom;
        m_delta += (event->pos() - m_reference) * 1.0/current_page->zoom_factor;
        //m_delta += (event->pos() - m_reference) * 1.0/current_zoom;
        m_reference = event->pos();
        update();
}
void ImageViewer::mouseReleaseEvent(QMouseEvent *) {
	qApp->restoreOverrideCursor();
        setMouseTracking(false);
}


void ImageViewer::setPage(page_data *page) {
	current_page = page;

	m_delta = page->position;
	//current_zoom = page->zoom_factor;

	setPixmap(*current_page->pixmap);
	//update();
}

void ImageViewer::setPixmap(const QPixmap &pix) {
	m_pixmap = pix;
        m_rect = m_pixmap.rect();
       
	m_rect.translate(-m_rect.center());
	
	current_zoom = 1;
        update();
}

// TODO: rethink this
void ImageViewer::scale(qreal s) {
        current_zoom = s;
        update();
}

void ImageViewer::wheelEvent(QWheelEvent * event) {
	
	// actualy scroll if cntl key pressed
	if (event->modifiers() & Qt::ControlModifier) {
	    QWidget::wheelEvent(event);
	} else {
		if (event->angleDelta().y() > 0) {
			current_page->zoom_factor *= ZOOM_FACTOR;
			//current_zoom *= ZOOM_FACTOR;
		} 
		else if (event->angleDelta().y() < 0){
			current_page->zoom_factor /= ZOOM_FACTOR;
			//current_zoom /= ZOOM_FACTOR;
		}

		if (current_page != nullptr) {
			current_zoom = current_page->zoom_factor;
			update();
			emit zoom_factor(current_zoom);
		}
	}

	update();
}

