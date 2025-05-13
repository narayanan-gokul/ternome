#include <alsa/asoundlib.h>
#include <stdio.h>

int main(void) {
	snd_pcm_t *pcm_handle;
	snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;
	snd_pcm_hw_params_t *hwparams;
	char *pcm_name;

	unsigned int rate = 44100;
	unsigned int exact_rate;
	int periods = 2;
	snd_pcm_uframes_t periodsize = 8192;
	
	pcm_name = strdup("plughw:0,0");
	snd_pcm_hw_params_alloca(&hwparams);

	// Opening the PCM device.
	if (snd_pcm_open(&pcm_handle, pcm_name, stream, 0) < 0) {
		fprintf(stderr, "Error opening PCM device %s.\n", pcm_name);
		return -1;
	}

	// Initializing configuration.
	if (snd_pcm_hw_params_any(pcm_handle, hwparams) < 0) {
		fprintf(stderr, "Cannot configure PCM device.\n");
		return -1;
	}

	// Set access type.
	if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
		fprintf(stderr, "Error setting access.\n");
		return -1;
	}

	// Set format.
	if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE) < 0) {
		fprintf(stderr, "Error setting format.\n");
		return -1;
	}

	// Set sample rate. if exact rate is not supported, use the supported rate.
	if (snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_rate, 0) < 0) {
		fprintf(stderr, "Error setting rate.\n");
		return -1;
	}

	if (rate != exact_rate) {
		fprintf(stderr,
			"The rate %d Hz is not supported by your hardware.\nUsing %d Hz instead.\n",
			rate,
			exact_rate
		);
	}

	// Set the number of channels.
	if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, 2) < 0) {
		fprintf(stderr, "Error setting channels.\n");
		return -1;
	}

	// Set the number of periods.
	if (snd_pcm_hw_params_set_periods(pcm_handle, hwparams, periods, 0) < 0) {
		fprintf(stderr, "Error setting periods.\n");
		return -1;
	}

	// Set buffer size in frames.
	if (snd_pcm_hw_params_set_buffer_size(pcm_handle, hwparams, (periodsize * periods)>>2) < 0) {
		fprintf(stderr, "Error setting buffersize.\n");
		return -1;
	}

	// Apply hw settings and prepare the device.
	if(snd_pcm_hw_params(pcm_handle, hwparams) < 0) {
		fprintf(stderr, "Error setting HW params.\n");
		return -1;
	}

	unsigned char *data;
	int pcmreturn, l1, l2;
	short s1, s2;
	int frames;

	data = (unsigned char *) malloc(periodsize);
	frames = periodsize >> 2;
	for (l1 = 0; l1 < 100; l1++) {
		for (l2 = 0; l2 < frames; l2++) {
			s1 = (l2 % 128) * 100 - 5000;
			s2 = (l2 % 256) * 100 - 5000;
			data[4 * 12] = (unsigned char) s1;
			data[4 * 12 + 1] = s1 >> 8;
			data[4 * 12 + 2] = (unsigned char) s2;
			data[4 * 12 + 3] = s2 >> 8;
		}
		while ((pcmreturn = snd_pcm_writei(pcm_handle, data, frames)) < 0) {
			snd_pcm_prepare(pcm_handle);
			fprintf(stderr, "<<<<<<<<<<<< Bufer underrun >>>>>>>>>>>>\n");
		}
	}

	return 0;
}
