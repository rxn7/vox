#include "vox/client/core/engine.hpp"
#include "game.hpp"

i32 main(i32 argc, char **argv) {
	Engine engine;
	if(!engine.init()) {
		return 1;
	}

	// NOTE: its wrote in this weird way to ensure destructor on Game gets called before the engine.
	// we could use heap allocation but i prefer to keep it on stack
	{
		Game game;
		std::println("Running the game");
		engine.run_game(&game);
	}

	return 0;
}
