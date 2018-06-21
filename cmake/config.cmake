macro(ubit_platform_core)

	target_link_libraries(ubit
		PUBLIC -lGL
		PUBLIC -lX11
		PUBLIC -ljpeg
	)

endmacro()

