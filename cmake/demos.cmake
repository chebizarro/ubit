add_executable(udemos
    examples/demos/udemos.cpp
    examples/demos/viewer.cpp
    examples/demos/gltriangle.cpp
    examples/demos/edi.cpp
    examples/demos/ima.cpp
    examples/demos/draw.cpp
    examples/demos/zoom.cpp
    examples/demos/zoomlib.cpp
    examples/demos/toolglass.cpp
    examples/demos/piemenus.cpp
    examples/demos/magiclens.cpp
    examples/demos/viewer.hpp
    examples/demos/gltriangle.hpp
    examples/demos/edi.hpp
    examples/demos/ima.hpp
    examples/demos/draw.hpp
    examples/demos/zoom.hpp
    examples/demos/zoomImpl.hpp
    examples/demos/zoomlib.hpp
    examples/demos/toolglass.hpp
    examples/demos/piemenus.hpp
    examples/demos/magiclens.hpp
)

ubit_add_include_dir(udemos)

ubit_add_libraries(udemos)
