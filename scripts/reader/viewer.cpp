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

        p.translate(rect().center());
        p.translate(m_delta*current_zoom);
        p.drawPixmap(m_rect.topLeft(), m_pixmap);
}
void SceneImageViewer::mousePressEvent(QMouseEvent *event) {
        m_reference = event->pos();
        qApp->setOverrideCursor(Qt::ClosedHandCursor);
        setMouseTracking(true);
}
void SceneImageViewer::mouseMoveEvent(QMouseEvent *event) {
        m_delta += (event->pos() - m_reference) * 1.0/current_zoom;
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
       
	m_rect.translate(-m_rect.center());
        update();
}
/*
void SceneImageViewer::scale(qreal s) {
        m_scale *= s;
        update();
}
*/

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
		} 
		else if (event->angleDelta().y() < 0){
			current_zoom /= ZOOM_FACTOR;
		}
		emit zoom_factor(current_zoom);
	}
}

