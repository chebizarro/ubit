add_library(ubit STATIC
    ${UBIT_CORE_FILES}
)

target_include_directories(ubit
    PRIVATE src
)

ubit_platform_core()
