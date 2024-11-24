#include <err.h>
#include <miniaudio.h>
#include <stdio.h>
#include <unistd.h>

#define ma_die(msg) errx(1, "%s: %s", msg, ma_result_description(result))

int main(void) {
	ma_engine engine = {0};
	ma_result result = ma_engine_init(NULL, &engine);
	if(result != MA_SUCCESS) ma_die("ma_engine_init failed");

	ma_sound sound_bell = {0};

	result = ma_sound_init_from_file(
		&engine, "bell.mp3", 0, NULL, NULL, &sound_bell
	);
	if(result != MA_SUCCESS) ma_die("load bell.mp3 failed");

	ma_uint64 length = 0;
	ma_sound_get_length_in_pcm_frames(&sound_bell, &length);

	ma_sound_start(&sound_bell);

	for(;; usleep(1000)) {
		ma_uint64 current = 0;
		ma_sound_get_cursor_in_pcm_frames(&sound_bell, &current);
		if(current >= length) break;
	}

	return 0;
}
