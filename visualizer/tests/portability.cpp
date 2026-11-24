#include "../filewatch.h"
#include "../music.h"

#include <chrono>
#include <cstdio>
#include <fstream>
#include <thread>

int main(int argc, char **argv) {
	if (argc != 2) return 1;

	MusicPlayer player;
	player.start(3.0);
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	if (player.get_time() < 3.01) return 1;
	player.set_time(1.0);
	if (player.get_time() < 1.0 || player.get_time() >= 2.0) return 1;
	player.stop();
	player.start(5.0);
	if (player.get_time() < 5.0 || player.get_time() >= 6.0) return 1;

	FileWatch watch(argv[1]);
	if (watch.changed()) return 1;
	{
		std::ofstream file(argv[1]);
		if (!file) return 1;
		file << "test";
	}
	bool detected = watch.changed();
	bool unchanged = !watch.changed();
	std::remove(argv[1]);
	return detected && unchanged ? 0 : 1;
}