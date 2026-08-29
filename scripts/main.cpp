#include <QtWidgets>
#include <iostream>
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qwidget.h>
#include <string>

#include "./global-variables.hpp"
#include "./reader/reader.hpp"

QDir main_dir;
std::string file_path;

class main_window : public QWidget {
	//Q_OBJECT
	
	public:
		main_window();
		
		QVBoxLayout main_layout;
		reader_component* rc;

};

main_window::main_window() :
	QWidget(),
	main_layout()
{
	setWindowTitle("PDF Reader");
	
	// commenting this makes the program not show the pdf
	//setWindowState(Qt::WindowMaximized);
	showMaximized(); // does the same as the function above
	//show();
	setLayout(&main_layout);
	main_layout.setContentsMargins(0, 0, 0, 0);
	
	// from main_window::showEvent
	if (QCoreApplication::arguments().size() > 1) {
		printf("Opening \"%s\"\n", QCoreApplication::arguments()[1].toStdString().c_str());
		file_path = QCoreApplication::arguments()[1].toStdString();
		
		rc = new reader_component();
		main_layout.addWidget(rc);
	};
}


int main(int argc, char * argv[]) {
	QApplication app(argc, argv);
	main_window window = main_window();
	
	main_dir = QDir(QCoreApplication::applicationDirPath());

	return app.exec();
}
