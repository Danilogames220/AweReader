#include <QtWidgets>
#include <iostream>
#include <qboxlayout.h>
#include <qnamespace.h>

#include "./global-variables.hpp"
#include "./reader/reader.hpp"

class main_window : public QWidget {
	protected:
		void showEvent(QShowEvent * event) override;
		void resizeEvent(QResizeEvent * event) override;

	public:
		main_window();
		QVBoxLayout * main_layout;

		QSharedPointer<reader_component> rc;

};

void main_window::resizeEvent(QResizeEvent * event) {
	QWidget::resizeEvent(event);
	puts("window resized");
	if (rc != nullptr)
		main_layout->addWidget(rc.data());
}
void main_window::showEvent(QShowEvent * event) {
	QWidget::showEvent(event);

	main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 0, 0, 0);
	
	if (QCoreApplication::arguments().size() > 1) {
		puts("argument detected");
		file_path = QCoreApplication::arguments()[1].toStdString();
		
		//window.setWindowTitle("PDF Reader");
		//window.showMaximized();
		
		rc = QSharedPointer<reader_component>(new reader_component(), &QWidget::deleteLater);
		//main_layout->addWidget(rc);
	};

};

main_window::main_window() {
	setWindowTitle("PDF Reader");
	
	setWindowState(Qt::WindowMaximized);
	//showMaximized();
	show();

	//setWindowTitle("PDF Reader");
	//showMaximized();
	//setWindowState(Qt::WindowMaximized);
	//show();
	//showMaximized();
}


int main(int argc, char * argv[]) {
	QApplication app(argc, argv);
	main_window window = main_window();
	
	main_dir = QDir(QCoreApplication::applicationDirPath());
	//QVBoxLayout main_layout(&window);

	/*
	if (QCoreApplication::arguments().size() > 1) {
		puts("argument detected");
		file_path = std::string(argv[1]);
		
		
		window.setWindowTitle("PDF Reader");
		window.showMaximized();
		
		reader_component rc = reader_component();
		main_layout.addWidget(&rc);
		return app.exec();
	};
	*/	


	return app.exec();
}
