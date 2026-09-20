// Source:
// https://stackoverflow.com/questions/40683840/zooming-and-panning-an-image-in-a-qscrollarea
#ifndef	VIEWER_HPP 
#define VIEWER_HPP

#include <QtCore>
#include <QtWidgets>
#include <QGraphicsView>
#include <qevent.h>
#include <qpixmap.h>

class SceneImageViewer : public QWidget {
	Q_OBJECT

	QPixmap m_pixmap;
	QRectF m_rect;
	QPointF m_reference;
	QPointF m_delta;
	qreal m_scale;
		      
	float current_zoom;
	protected:
		void paintEvent(QPaintEvent *) override;
		void mousePressEvent(QMouseEvent *event) override;
		void mouseMoveEvent(QMouseEvent *event) override;
		void mouseReleaseEvent(QMouseEvent *) override;
		void wheelEvent(QWheelEvent * event) override;
	
	public:
		SceneImageViewer();	

		void setPixmap(const QPixmap &pix);
		void scale(qreal s);
	signals:
		void zoom_factor(float factor);
};


#endif
