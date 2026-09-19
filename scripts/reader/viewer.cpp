#include <QtCore>
#include <QtWidgets>
#include <QGraphicsView>
#include "./viewer.hpp"

#include <iostream>
#include <qevent.h>
#include <qnamespace.h>

#define ZOOM_FACTOR 1.5

SceneImageViewer::SceneImageViewer() {
	m_scale = 1.;
	current_zoom = 1.;
};

void SceneImageViewer::paintEvent(QPaintEvent *) {
        QPainter p{this};
	auto t_rect = rect();
	t_rect.setSize(t_rect.size()*(1/current_zoom));

        p.translate(t_rect.center());
        //p.scale(current_zoom, current_zoom);
        //p.scale(m_scale, m_scale);
        p.translate(m_delta);
        p.drawPixmap(m_rect.topLeft(), m_pixmap);
}
void SceneImageViewer::mousePressEvent(QMouseEvent *event) {
        m_reference = event->pos();
        qApp->setOverrideCursor(Qt::ClosedHandCursor);
        setMouseTracking(true);
}
void SceneImageViewer::mouseMoveEvent(QMouseEvent *event) {
        //m_delta += (event->pos() - m_reference) * 1.0/current_zoom;
        m_delta += (event->pos() - m_reference) * 1.0/m_scale;
        m_reference = event->pos();
        update();
}
void SceneImageViewer::mouseReleaseEvent(QMouseEvent *) {
	qApp->restoreOverrideCursor();
        setMouseTracking(false);
}

void SceneImageViewer::setPixmap(const QPixmap &pix) {
        m_pixmap = pix;
        m_rect = m_pixmap.rect();
	//current_zoom = 1;
	//QRect t_rect = m_rect;
	//t_rect.setSize(t_rect.size()*(1/current_zoom));
        
	m_rect.translate(-m_rect.center());
        update();
}
void SceneImageViewer::scale(qreal s) {
        m_scale *= s;
        update();
}

void SceneImageViewer::wheelEvent(QWheelEvent * event) {
	
	// actualy scroll if cntl key pressed
	if (event->modifiers() & Qt::ControlModifier) {
	    QWidget::wheelEvent(event);
	} else {
		//std::cout << "scroll event: "
		//     << event->angleDelta().y()
		//     << "\n";
		if (event->angleDelta().y() > 0) {
			current_zoom *= ZOOM_FACTOR;
			//scale(ZOOM_FACTOR);
		} 
		else if (event->angleDelta().y() < 0){
			current_zoom /= ZOOM_FACTOR;
			//scale(1./ZOOM_FACTOR);
		}
		emit zoom_factor(current_zoom);
	}
}



/*
SceneImageViewer::SceneImageViewer() {
	current_zoom = 1.;
	default_x = -1;
	default_y = -1;

	setScene(&m_scene);
	m_scene.addItem(&m_item);
	setDragMode(QGraphicsView::ScrollHandDrag);
	// hide scrollbars
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setResizeAnchor(QGraphicsView::AnchorViewCenter);
	
	//setAlignment(Qt::AlignCenter);
	//setResizeAnchor(QGraphicsView::NoAnchor);
}

void SceneImageViewer::setPixmap(const QPixmap &Pixmap) {
	//setResizeAnchor(QGraphicsView::AnchorViewCenter);
	//setResizeAnchor(QGraphicsView::NoAnchor);
	
	//horizontalScrollBar()->setMaximum(Pixmap.size().width() * 1.5);
	//horizontalScrollBar()->setMinimum(0);
	//verticalScrollBar()->setMaximum(100000);
	//verticalScrollBar()->setMinimum(-100000);
	//m_item.setOffset(0,0);
	
	
	printf("\nhori.s: min: %d max: %d val: %d\n", 
		horizontalScrollBar()->minimum(),
		horizontalScrollBar()->maximum(),
		horizontalScrollBar()->value()
	);
	printf("vert.s: min: %d max: %d val: %d\n", 
		verticalScrollBar()->minimum(),
		verticalScrollBar()->maximum(),
		verticalScrollBar()->value()
	);
	printf("page: width: %d height: %d\n",
		Pixmap.width(),
		Pixmap.height()
	);
	printf("default_x %d default_y: %d\n",
		default_x,
		default_y
	);
	
	printf("offset x %f offset y: %f\n",
		m_item.offset().x(),
		m_item.offset().y()
	);

	//m_item.setOffset(0,0);
	//horizontalScrollBar()->setMaximum(10000);
	horizontalScrollBar()->setMinimum(0);
	//verticalScrollBar()->setMaximum(10000);
	verticalScrollBar()->setMinimum(0);

	
	horizontalScrollBar()->setMaximum(Pixmap.width() + 1);
	verticalScrollBar()->setMaximum(Pixmap.height() + 1);

	horizontalScrollBar()->setValue(0);
	verticalScrollBar()->setValue(0);
	

	//horizontalScrollBar()->setValue(-100);
	//horizontalScrollBar()->setValue(-100);
	//fitInView(m_item);
	//m_item.set
	
	//m_item.setOffset(0,0);
	//setSceneRect(0,0,0,0);
	pixmap = Pixmap;
	m_item.setPixmap(pixmap);
	//setSceneRect(m_item.sceneBoundingRect());
	
	horizontalScrollBar()->setValue(100);
	//centerImage();
	
	//if (default_x == -1 || default_y == -1) {
	//	auto offset = -QRectF(pixmap.rect()).center();
	//	m_item.setOffset(offset);
	//	setSceneRect(offset.x()*4, offset.y()*4, -offset.x()*8, -offset.y()*8);
	//	translate(1, 1);
	//	default_x = horizontalScrollBar()->value();
	//	default_y = verticalScrollBar()->value();
	//}
}

void SceneImageViewer::centerImage() {
	horizontalScrollBar()->setValue(100);

	//verticalScrollBar()->setValue(default_y);
	//horizontalScrollBar()->setValue(default_x);
}
void SceneImageViewer::scale(qreal s) { 
	QGraphicsView::scale(s, s); 
}

void SceneImageViewer::wheelEvent(QWheelEvent * event)
{
	
	// actualy scroll if cntl key pressed
	if (event->modifiers() & Qt::ControlModifier) {
	    QGraphicsView::wheelEvent(event);
	} else {
		//std::cout << "scroll event: "
		//     << event->angleDelta().y()
		//     << "\n";
		if (event->angleDelta().y() > 0) {
			current_zoom *= ZOOM_FACTOR;
			//scale(ZOOM_FACTOR);
		} 
		else if (event->angleDelta().y() < 0){
			current_zoom /= ZOOM_FACTOR;
			//scale(1./ZOOM_FACTOR);
		}
		emit zoom_factor(current_zoom);
	}
}

void SceneImageViewer::mouseMoveEvent(QMouseEvent * event) {
	puts("image dragged");
	
	// sort of does nothing
	//m_item.setOffset(horizontalScrollBar()->value(), verticalScrollBar()->value());
	//horizontalScrollBar()->setValue(0);
	//verticalScrollBar()->setValue(0);

	QGraphicsView::mouseMoveEvent(event);
}
*/
