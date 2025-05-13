#include <alsa/asoundlib.h>

int main(void) {
	int rc;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;
	unsigned int val1, val2;
	int dir;
	snd_pcm_uframes_t frames;

	rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (rc < 0) {
		fprintf(stderr, "Unable to open PCM device: %s.\n", snd_strerror(rc));
		return 1;
	}

	snd_pcm_hw_params_alloca(&params);
	snd_pcm_hw_params_any(handle, params);
	snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_channels(handle, params, 2);

	val1 = 44100;
	snd_pcm_hw_params_set_rate_near(handle, params, &val1, &dir);

	rc = snd_pcm_hw_params(handle, params);
	if (rc < 0) {
		fprintf(stderr, "Unable to set hw parameters: %s\n", snd_strerror(rc));
		return 1;
	}
	snd_pcm_close(handle);
	return 0;
}
