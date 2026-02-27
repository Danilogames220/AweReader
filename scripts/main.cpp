#include <QtCore>
#include <QtWidgets>
#include <iostream>

#include "./main.hpp"
#include "./global-variables.hpp"
#include "./reader/reader.hpp"

void main_window::resizeEvent(QResizeEvent * event) {
	QWidget::resizeEvent(event);
	
	res_count++;
	printf("%d\n", res_count);

	if (res_count == 2) {
		puts("run main");
		emit start();
		first_resize = true;
	}
}
void main_window::showEvent(QShowEvent * event) {
	QWidget::showEvent(event);
};

main_window::main_window() {
	setWindowTitle("PDF Reader");
	setWindowState(Qt::WindowMaximized);
	
	QObject::connect(this, &main_window::start, this, &main_window::main);
	show();
}


int main(int argc, char * argv[]) {
	QApplication app(argc, argv);
	main_window window = main_window();
	
	main_dir = QDir(QCoreApplication::applicationDirPath());
	
	return app.exec();
}

void main_window::main(void) {
	
	main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 0, 0, 0);
	
	if (QCoreApplication::arguments().size() > 1) {
		puts("argument detected");
		file_path = QCoreApplication::arguments()[1].toStdString();
		
		//window.setWindowTitle("PDF Reader");
		//window.showMaximized();
		
		rc = QSharedPointer<reader_component>(new reader_component(), &QWidget::deleteLater);
		//main_layout->addWidget(rc);
		if (rc != nullptr)
			main_layout->addWidget(rc.data());
	};
}
