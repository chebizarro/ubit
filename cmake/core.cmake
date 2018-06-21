add_library(ubit STATIC
    ${UBIT_CORE_FILES}
)

ubit_add_include_dir(ubit)

ubit_platform_core()
