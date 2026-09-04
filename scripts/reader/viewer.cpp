#include <QtCore>
#include <QtWidgets>
#include <QGraphicsView>
#include "./viewer.hpp"

#include <iostream>

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
	//setResizeAnchor(QGraphicsView::NoAnchor);
	
}

void SceneImageViewer::setPixmap(const QPixmap &Pixmap) {
	//setResizeAnchor(QGraphicsView::AnchorViewCenter);
	//setResizeAnchor(QGraphicsView::NoAnchor);
	
	horizontalScrollBar()->setMaximum(100000);
	horizontalScrollBar()->setMinimum(-100000);
	verticalScrollBar()->setMaximum(100000);
	verticalScrollBar()->setMinimum(-100000);
	
	pixmap = Pixmap;
	m_item.setPixmap(pixmap);

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
