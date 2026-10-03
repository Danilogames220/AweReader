// Source:
// https://stackoverflow.com/questions/40683840/zooming-and-panning-an-image-in-a-qscrollarea
#ifndef	VIEWER_HPP 
#define VIEWER_HPP

#include <QtCore>
#include <QtWidgets>
#include <QGraphicsView>

#include "../global-variables.hpp"

class ImageViewer : public QWidget {
	Q_OBJECT
	
	QPointF * current_pos;

	QPixmap m_pixmap;
	QRectF m_rect;
	QPointF m_reference;
	// current offset of the reader
	QPointF m_delta;
		      
	protected:
		void paintEvent(QPaintEvent *) override;
		void mousePressEvent(QMouseEvent *event) override;
		void mouseMoveEvent(QMouseEvent *event) override;
		void mouseReleaseEvent(QMouseEvent *) override;
		void wheelEvent(QWheelEvent * event) override;
	
	public:
		float current_zoom;
		page_data * current_page;
		
		ImageViewer();	

		void setPage(page_data *page);
		void setPixmap(const QPixmap &pix);
		void scale(qreal s);
	signals:
		void page_changed();
		void query_zoom(float factor);

		void zoom_factor(float factor);
};


#endif
