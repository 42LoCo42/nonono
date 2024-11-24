#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <unistd.h>

#define TEMPERATURE_FILE "/sys/devices/platform/coretemp.0/hwmon/hwmon1/temp1_input"
#define PWM_FILE "/sys/devices/pci0000:00/0000:00:14.0/usb3/3-5/3-5:1.1/0003:0C70:F011.0005/hwmon/hwmon4/pwm1"

int main(int argc, char** argv) {
	if(argc < 4) {
		errx(1, "Usage: %s <minTemp> <maxTemp> <interval>", argv[0]);
	}

	// TODO: use strtol/strtod for error handling
	int min = atoi(argv[1]);
	int max = atoi(argv[2]);
	int interval = atoi(argv[3]);

	double m = 255.0 / (max - min);
	double n = -m * min;

	FILE* temperatureFile = fopen(TEMPERATURE_FILE, "r");
	if(!temperatureFile) err(1, "Could not open temperature file %s", TEMPERATURE_FILE);
	setbuf(temperatureFile, NULL);

	FILE* pwmFile = fopen(PWM_FILE, "w");
	if(flock(fileno(pwmFile), LOCK_EX) < 0) err(1, "Could not lock PWM file %s", PWM_FILE);
	if(!pwmFile) err(1, "Could not open PWM file %s", PWM_FILE);

	for(;;) {
		int current;
		if(fseek(temperatureFile, 0, SEEK_SET) < 0)
			err(1, "Could not seek in temperature file");
		if(fscanf(temperatureFile, "%d", &current) < 1)
			err(1, "Could not read temperature");

		int pwm = m * current / 1000 + n;
		if(pwm > 255) pwm = 255;
		printf("current temperature: %d°C, PWM of %d\n", current / 1000, pwm);

		if(fseek(pwmFile, 0, SEEK_SET) < 0)
			err(1, "Could not seek in PWM file");
		if(fprintf(pwmFile, "%d\n", pwm) < 1)
			err(1, "Could not write PWM");

		sleep(interval);
	}
}
