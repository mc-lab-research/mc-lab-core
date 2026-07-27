# ==============================================================================
# MC-LAB-CORE Command-Line Product
# ==============================================================================

# The CLI is a repository product composed from the public Promela frontend and
# runtime contracts. Its implementation and product-level smoke tests remain in
# separate directories so packaging never acquires a dependency on test code.

include(GNUInstallDirs)

add_subdirectory(
    src
)

if(PROJECT_IS_TOP_LEVEL AND BUILD_TESTING)
    add_subdirectory(
        tests
    )
endif()
