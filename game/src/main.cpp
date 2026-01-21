#include "vox/core/engine.hpp"
#include "game.hpp"

i32 main(i32 argc, char **argv) {
    std::unique_ptr<Game> game = std::make_unique<Game>();

	Engine engine(std::move(game));
	engine.run();

	return 0;
}
