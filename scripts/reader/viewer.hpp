// Source:
// https://stackoverflow.com/questions/40683840/zooming-and-panning-an-image-in-a-qscrollarea
#ifndef	VIEWER_HPP 
#define VIEWER_HPP

#include <QtCore>
#include <QtWidgets>
#include <QGraphicsView>
#include <qpixmap.h>

class SceneImageViewer : public QGraphicsView {
	Q_OBJECT

	QGraphicsScene m_scene;
	QGraphicsPixmapItem m_item;
	QPixmap pixmap;	

	int default_x, default_y;

	float current_zoom;

	protected:
		virtual void wheelEvent(QWheelEvent * event);
	public:
		SceneImageViewer();
		
		void setPixmap(const QPixmap &Pixmap);
		void centerImage();
		
		void scale(qreal s);
	
	//public slots:
	signals:
		void zoom_factor(float factor);
};

#endif
