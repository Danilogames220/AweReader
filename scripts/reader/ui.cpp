#include "./reader.hpp"
#include "../global-variables.hpp"

reader_component::reader_component(QWidget * parent) :
	QWidget(parent),
	reader_c_layout(this),

	// top panel
	top_panel(),
	top_layout(&top_panel),


	back_button("<"),
	current_path_label(QString::fromStdString(file_path)),

	// pages
	pages_container(),

	// options
	bottom_buttons(),
	bb_layout(&bottom_buttons),

	prev_button("<-"),
	next_button("->"),
	current_page(QString::fromStdString(std::format("{} / {}", current_page_index + 1, page_count)))
		
{
	// load ui
	reader_c_layout.setContentsMargins(0, 0, 0, 0);
	setLayout(&reader_c_layout);
	reader_c_layout.addWidget(&top_panel);
	reader_c_layout.addWidget(&pages_container);
	reader_c_layout.addWidget(&bottom_buttons);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	// top panel
	top_panel.show();

	back_button.setFixedSize(30, 30);
	back_button.setFocusPolicy(Qt::FocusPolicy::NoFocus);
		
	top_layout.addWidget(&back_button);
	top_layout.addWidget(&current_path_label);

	// pages
	pages_container.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	pages_container.show();

	// bottom options
	bb_layout.setContentsMargins(0, 0, 0, 0);
	bottom_buttons.show();

	prev_button.setFocusPolicy(Qt::FocusPolicy::NoFocus);
	next_button.setFocusPolicy(Qt::FocusPolicy::NoFocus);

	current_page.setAlignment(Qt::AlignCenter);

	bb_layout.addWidget(&prev_button);
	bb_layout.addWidget(&current_page);
	bb_layout.addWidget(&next_button);
};

