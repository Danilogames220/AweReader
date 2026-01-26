#include <gtkmm.h>
#include "gtkmm/text.h"
#include <iostream>

#include "./global-variables.hpp"
#include "./reader/reader.hpp"

using namespace std;

class MyWindow : public Gtk::Window {
	public:
  		MyWindow();
		Gtk::Box content_box;
		Gtk::Button btn;
		Gtk::Button btn2;
		Gtk::Entry path_in;

		reader_component reader;

		void click() {
			printf("%s\n", path_in.get_text().c_str());
		};
};

MyWindow::MyWindow():
	content_box(Gtk::Orientation::VERTICAL),
	path_in(),
	btn("text"), 
	btn2("text2")

{
  
	set_title("PDF Reader");
  	set_default_size(800, 600);

	set_child(reader);

	/*
	//btn.set_margin(10);
  	btn.set_size_request(-1, 20);
	btn.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::click));
	
	btn2.set_margin(10);

	path_in.set_placeholder_text("huh");

	content_box.append(btn);
	content_box.append(btn2);
	content_box.append(path_in);
	*/
}


int main(int argc, char * argv[]) {
	// get path to read pdf
	auto app = Gtk::Application::create("org.gtkmm.examples.base");

	return app->make_window_and_run<MyWindow>(argc, argv);;
}
