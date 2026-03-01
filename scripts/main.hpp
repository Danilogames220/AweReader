#ifndef MAIN_HPP
#define MAIN_HPP

#include <QtCore>
#include <QtWidgets>
#include <qevent.h>

#include "./reader/reader.hpp"

class main_window : public QWidget {
	Q_OBJECT

	protected:
		void showEvent(QShowEvent * event) override;
		void resizeEvent(QResizeEvent * event) override;
		void keyPressEvent(QKeyEvent * event) override;
	public:
		main_window();
		QVBoxLayout * main_layout;

		QSharedPointer<reader_component> rc;

		void main(void);
	
	private:
		bool first_resize;
		char res_count = 0;

	signals:
		void start(void);
};


#endif
