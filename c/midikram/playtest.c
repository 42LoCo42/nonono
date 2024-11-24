#include <fluidsynth.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void pe(int code) {
	printf("%s\n", code == FLUID_OK ? "CALL OK" : "CALL ERROR");
	fflush(stdout);
}

static void inspectsoundfont(fluid_synth_t* synth, int font_id) {
	fluid_sfont_t* sfont = fluid_synth_get_sfont_by_id(synth, font_id);
	if(sfont == NULL) return;

	for(int bank = 0; bank < 16384; bank++) {
		for(int num = 0; num < 128; num++) {
			fluid_preset_t* preset = fluid_sfont_get_preset(sfont, bank, num);
			if(preset == NULL) continue;
			const char* name = fluid_preset_get_name(preset);
			printf("bank: %d, preset: %d, name: %s\n", bank, num, name);
		}
	}
}

int main() {
	fluid_settings_t* settings = new_fluid_settings();
	fluid_settings_setstr(settings, "audio.driver", "pulseaudio");

	fluid_synth_t* synth = new_fluid_synth(settings);
	fluid_synth_sfload(synth, "/usr/share/soundfonts/FluidR3_GM.sf2", 1);
	fluid_synth_program_select(synth, 0, 1, 0, 0);

	// inspectsoundfont(synth, 1);

	fluid_audio_driver_t* adriver = new_fluid_audio_driver(settings, synth);

	int notes[] = {
		60, 62, 64, 65, 67, 69, 71, 72
	};
	size_t len = sizeof(notes) / sizeof(notes[0]);

	for(size_t i = 0; i < len; ++i) {
		fluid_synth_noteon(synth, 0, notes[i], 100);
		usleep(500000);
		fluid_synth_noteoff(synth, 0, notes[i]);
	}

	usleep(500000);
	fluid_synth_all_notes_off(synth, 0);
	usleep(500000);

	delete_fluid_audio_driver(adriver);
	delete_fluid_synth(synth);
	delete_fluid_settings(settings);
}
