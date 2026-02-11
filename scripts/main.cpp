#include <QtWidgets>
#include <iostream>

#include "./global-variables.hpp"
#include "./reader/reader.hpp"

class main_window : public QWidget {
	public:
		main_window();
};

main_window::main_window()
{
	resize(800, 600);
	setWindowTitle("PDF Reader");

	//set_child(reader);
	show();
}

QDir main_dir;

int main(int argc, char * argv[]) {
	QApplication app(argc, argv);
	main_window window = main_window();
	
	main_dir = QDir(QCoreApplication::applicationDirPath());

	//file_path = "./doc.pdf";
	

	reader_component rc = reader_component(&window);


	return app.exec();
}
