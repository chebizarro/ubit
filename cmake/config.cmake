macro(ubit_platform_core)

	target_link_libraries(ubit
		PUBLIC -lGL
		PUBLIC -lX11
		PUBLIC -ljpeg
	)

endmacro()

macro(ubit_add_libraries _PACKAGE)

	target_link_libraries(${_PACKAGE}
		PRIVATE ubit
		PUBLIC -lGL
		PUBLIC -lX11
		PUBLIC -lpthread
		PUBLIC -lglut
		PUBLIC -lGLU
		PUBLIC -lXmu
		PUBLIC -lfreetype
		PUBLIC -lftgl
		PUBLIC -lXpm
		PUBLIC -ljpeg
		PUBLIC -lgif
	)

endmacro()

macro(ubit_add_include_dir _PACKAGE)

	target_include_directories(${_PACKAGE}
		PRIVATE include
		PRIVATE src
		PUBLIC /usr/include/freetype2
	)


endmacro()
