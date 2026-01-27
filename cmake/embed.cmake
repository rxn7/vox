set(B_PRODUCTION_MODE ON CACHE BOOL "" FORCE)

FetchContent_Declare(
	battery-embed
	GIT_REPOSITORY https://github.com/batterycenter/embed.git
	GIT_TAG v1.2.19
	GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(battery-embed)
