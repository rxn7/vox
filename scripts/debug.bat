@Echo off

cmake --build build --config Debug && (
	pushd build\client\game
	.\Debug\vox_game.exe
	popd
) || (
	echo Failed to build with debug config.
) 