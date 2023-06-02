#include "gp_audio_output.h"
#include "bass.h"

static struct Plugin plugins[] = {
		{"./bassflac.dll", 0}
};

static uint8_t plugins_size = sizeof(plugins) / sizeof(struct Plugin);

enum GpResult gp_audio_output_init(enum GpSampleRate sample_rate) {
	for (uint8_t i = 0; i < plugins_size; i++) {
		plugins[i].handle = BASS_PluginLoad(plugins[i].path, 0);
		if (plugins[i].handle == 0) return GP_RESULT_ERROR;
	}

	if (!BASS_Init(-1, sample_rate, 0, NULL, NULL)) return GP_RESULT_ERROR;

	return GP_RESULT_OK;

}

enum GpResult gp_audio_output_close(void) {
	for (uint8_t i = 0; i < plugins_size; i++) {
		if (!BASS_PluginFree(plugins[i].handle)) return GP_RESULT_ERROR;
		plugins[i].handle = 0;
	}

	if (!BASS_Free()) return GP_RESULT_ERROR;

	return GP_RESULT_OK;
}
