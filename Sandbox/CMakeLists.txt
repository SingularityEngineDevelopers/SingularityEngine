add_executable(Singularity_SandboxApp "main.cpp")
target_link_libraries(
	Singularity_SandboxApp
	PRIVATE SingularityEngine
)

add_custom_command(
	TARGET Singularity_SandboxApp POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3::SDL3> $<TARGET_FILE_DIR:Singularity_SandboxApp>
)