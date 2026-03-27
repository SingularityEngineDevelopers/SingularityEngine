set(SNGL_SRC_PREFIX "${SNGL_ROOT_DIR}/Engine/src" CACHE STRING "" FORCE)
set(SNGL_ALLOCATORS_TEST_SOURCE 
	${SNGL_SRC_PREFIX}/Core/Math.h
	${SNGL_SRC_PREFIX}/Platform/OS.h
	${SNGL_SRC_PREFIX}/Platform/OS_win32.cpp
	${SNGL_SRC_PREFIX}/Core/LinearArenaAllocator.h
	${SNGL_SRC_PREFIX}/Core/LinearArenaAllocator.cpp
)

add_executable(sngl_test_allocators Allocators.cpp ${SNGL_ALLOCATORS_TEST_SOURCE})

target_link_libraries(sngl_test_allocators 
	PRIVATE gtest_main SingularityEngine
)

target_include_directories(sngl_test_allocators
	PRIVATE ${SNGL_SRC_PREFIX}
)