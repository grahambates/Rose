
#include <chrono>

class MusicPlayer {
	unsigned samplepos = 0;
	unsigned n_samples = 0;
	double time_offset = 0.0;
	double sample_rate = 44100.0;

	short *wavdata = nullptr;
	void *stream = nullptr;
	bool audio_initialized = false;
	std::chrono::steady_clock::time_point clock_start;
	double clock_time = 0.0;

public:
	MusicPlayer();
	~MusicPlayer();

	void load(const char *wav_filename);
	double length();
	void set_time(double time);
	double get_time();
	void start(double time);
	void stop();

	int callback(void *output, unsigned long frameCount, double outputTime);
};
