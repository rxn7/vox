@Echo off

cmake --build build --config Debug && (
	pushd build\client
	.\Debug\vox.exe
	popd
) || (
	echo Failed to build with debug config.
) 
