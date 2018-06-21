add_executable(udemos
    demos/udemos.cpp
    demos/viewer.cpp
    demos/gltriangle.cpp
    demos/edi.cpp
    demos/ima.cpp
    demos/draw.cpp
    demos/zoom.cpp
    demos/zoomlib.cpp
    demos/toolglass.cpp
    demos/piemenus.cpp
    demos/magiclens.cpp
    demos/viewer.hpp
    demos/gltriangle.hpp
    demos/edi.hpp
    demos/ima.hpp
    demos/draw.hpp
    demos/zoom.hpp
    demos/zoomImpl.hpp
    demos/zoomlib.hpp
    demos/toolglass.hpp
    demos/piemenus.hpp
    demos/magiclens.hpp
)

ubit_add_include_dir(udemos)

ubit_add_libraries(udemos)
