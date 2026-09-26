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
        p.translate(m_delta*current_zoom);
	p.drawPixmap(m_rect.topLeft(), m_pixmap);
}
void ImageViewer::mousePressEvent(QMouseEvent *event) {
        m_reference = event->pos();
        qApp->setOverrideCursor(Qt::ClosedHandCursor);
        setMouseTracking(true);
}
void ImageViewer::mouseMoveEvent(QMouseEvent *event) {
        //current_page->position += (event->pos() - m_reference) * 1.0/current_zoom;
        m_delta += (event->pos() - m_reference) * 1.0/current_zoom;
        m_reference = event->pos();
        update();
}
void ImageViewer::mouseReleaseEvent(QMouseEvent *) {
	qApp->restoreOverrideCursor();
        setMouseTracking(false);
}


void ImageViewer::setPage(page_data *page) {
	if (current_page != nullptr) {
		current_page->position = m_delta;
	}	

	current_page = page;
	m_delta = current_page->position;

	setPixmap(*current_page->pixmap);
}

void ImageViewer::setPixmap(const QPixmap &pix) {
        m_pixmap = pix;
        m_rect = m_pixmap.rect();
       
	m_rect.translate(-m_rect.center());
        update();
}
/*
void ImageViewer::scale(qreal s) {
        m_scale *= s;
        update();
}
*/

void ImageViewer::wheelEvent(QWheelEvent * event) {
	
	// actualy scroll if cntl key pressed
	if (event->modifiers() & Qt::ControlModifier) {
	    QWidget::wheelEvent(event);
	} else {
		//std::cout << "scroll event: "
		//     << event->angleDelta().y()
		//     << "\n";
		if (event->angleDelta().y() > 0) {
			current_zoom *= ZOOM_FACTOR;
		} 
		else if (event->angleDelta().y() < 0){
			current_zoom /= ZOOM_FACTOR;
		}
		emit zoom_factor(current_zoom);
	}
}

