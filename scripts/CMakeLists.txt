# reader source files
file(GLOB reader_SRC
	"reader/*.cpp"
)

target_sources ( ${PROJECT_NAME}
	PRIVATE
		"main.cpp"
		
		${reader_SRC}
)
