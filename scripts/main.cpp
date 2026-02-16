#include <QtWidgets>
#include <iostream>

#include "./global-variables.hpp"
#include "./reader/reader.hpp"

class main_window : public QWidget {
	public:
		main_window();
};

main_window::main_window() {
	resize(800, 600);
	setWindowTitle("PDF Reader");

	show();
}


int main(int argc, char * argv[]) {
	QApplication app(argc, argv);
	main_window window = main_window();
	
	main_dir = QDir(QCoreApplication::applicationDirPath());
	QVBoxLayout main_layout(&window);
	main_layout.setContentsMargins(0, 0, 0, 0);

	if (QCoreApplication::arguments().size() > 1) {
		puts("argument detected");
		file_path = std::string(argv[1]);
		
		reader_component rc = reader_component();
		main_layout.addWidget(&rc);
		return app.exec();
	};
	


	return app.exec();
}
