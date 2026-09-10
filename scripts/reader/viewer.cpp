#include <QtCore>
#include <QtWidgets>
#include <QGraphicsView>
#include "./viewer.hpp"

#include <iostream>
#include <qnamespace.h>

#define ZOOM_FACTOR 1.5

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
	
	setAlignment(Qt::AlignCenter);
	//setResizeAnchor(QGraphicsView::NoAnchor);
	
}

void SceneImageViewer::setPixmap(const QPixmap &Pixmap) {
	//setResizeAnchor(QGraphicsView::AnchorViewCenter);
	//setResizeAnchor(QGraphicsView::NoAnchor);
	
	/*
	horizontalScrollBar()->setMaximum(Pixmap.size().width() * 1.5);
	horizontalScrollBar()->setMinimum(0);
	verticalScrollBar()->setMaximum(100000);
	verticalScrollBar()->setMinimum(-100000);
	*/
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

	m_item.setOffset(0,0);
	//m_item.setOffset(0,0);
	horizontalScrollBar()->setMaximum(10000);
	horizontalScrollBar()->setMinimum(0);
	verticalScrollBar()->setMaximum(10000);
	verticalScrollBar()->setMinimum(0);

	horizontalScrollBar()->setValue(0);
	verticalScrollBar()->setValue(0);

	pixmap = Pixmap;
	m_item.setPixmap(pixmap);
	
	;

	if (default_x == -1 || default_y == -1) {
		auto offset = -QRectF(pixmap.rect()).center();
		m_item.setOffset(offset);
		setSceneRect(offset.x()*4, offset.y()*4, -offset.x()*8, -offset.y()*8);
		translate(1, 1);
		default_x = horizontalScrollBar()->value();
		default_y = verticalScrollBar()->value();
	}
}

void SceneImageViewer::centerImage() {
	verticalScrollBar()->setValue(default_y);
	horizontalScrollBar()->setValue(default_x);
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
		/*
		std::cout << "scroll event: "
		     << event->angleDelta().y()
		     << "\n";
		*/
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
