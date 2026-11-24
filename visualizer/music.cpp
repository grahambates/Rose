
#include <portaudio.h>

#include <algorithm>
#include <cstdio>
#include <cstring>

#include "music.h"

#define STREAM_BUFFER_SIZE 512

struct WavHeader {
	unsigned riff_tag;
	unsigned file_length;
	unsigned wave_tag;
	unsigned fmt_tag;
	unsigned header_length;
	unsigned short format;
	unsigned short num_channels;
	unsigned sample_rate;
	unsigned byte_rate;
	unsigned short block_align;
	unsigned short bits;
	unsigned data_tag;
	unsigned data_length;
};


int stream_callback(const void *input, void *output, unsigned long frameCount,
					const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
	MusicPlayer *player = (MusicPlayer *) userData;
	return player->callback(output, frameCount, timeInfo->outputBufferDacTime);
}

int MusicPlayer::callback(void *output, unsigned long frameCount, double outputTime) {
	time_offset = outputTime - samplepos / sample_rate;
	int remaining = n_samples - samplepos;
	int copy = std::max(0, std::min((int)frameCount, remaining));
	memset(output, 0, frameCount * 4);
	if (copy > 0) memcpy(output, &wavdata[samplepos * 2], copy * 4);
	samplepos += frameCount;
	return paContinue;
}


MusicPlayer::MusicPlayer() {
}

MusicPlayer::~MusicPlayer() {
	if (stream) Pa_CloseStream(stream);
	if (audio_initialized) Pa_Terminate();
	if (wavdata) free(wavdata);
}

void MusicPlayer::load(const char *wav_filename) {
	if (Pa_Initialize() != paNoError) {
		printf("Error initializing audio!\n");
		exit(1);
	}
	audio_initialized = true;
	FILE *fp = fopen(wav_filename,"rb");
	if (!fp) {
		printf("Unable to open music file %s\n", wav_filename);
		exit(1);
	}

	WavHeader wh;
	fread(&wh, sizeof(wh), 1, fp);
	if (wh.num_channels != 2 || wh.bits != 16) {
		printf("Music must be 16 bit stereo.\n");
		exit(1);
	}

	wavdata = (short *) malloc(wh.data_length);
	fread(wavdata, 1, wh.data_length, fp);
	fclose(fp);

	sample_rate = wh.sample_rate;
	n_samples = wh.data_length / 4;
}

double MusicPlayer::length() {
	return n_samples / sample_rate;
}

void MusicPlayer::set_time(double time) {
	if (!wavdata) {
		clock_start = std::chrono::steady_clock::now();
		clock_time = time;
		return;
	}
	samplepos = (int) (time * sample_rate);
	time_offset = Pa_GetStreamTime(stream) - time;
}

double MusicPlayer::get_time() {
	if (!wavdata) {
		return clock_time + std::chrono::duration<double>(std::chrono::steady_clock::now() - clock_start).count();
	}
	return Pa_GetStreamTime(stream) - time_offset;
}

void MusicPlayer::start(double time) {
	if (wavdata && !stream) {
		if (Pa_OpenDefaultStream(&stream, 0, 2, paInt16, sample_rate, STREAM_BUFFER_SIZE, stream_callback, this)) {
			printf("Error opening audio output!\n");
			exit(1);
		}
	}

	set_time(time);
	if (wavdata) Pa_StartStream(stream);
}

void MusicPlayer::stop() {
	if (wavdata) Pa_StopStream(stream);
}
