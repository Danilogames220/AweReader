#ifndef GLOBAL_VARIABLES_HPP
#define GLOBAL_VARIABLES_HPP

#include <QtWidgets>

#include <string>

// path of the pdf
extern std::string file_path;
// dir where the executable is in
extern QDir main_dir; 
// main dir where the app will pull the files from
extern QDir homepage_dir; 

// Used in the reader
class page_data {
	public:
		// the number that the page size was multiplied to be the size of the page
		float zoom_factor;
		// page number
		int index;
		QPixmap * label_pix;


};

#endif
