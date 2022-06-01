#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "macros.h"

// reads temperature of each package/socket
// allocates memory for the measurements
double* cpu_temp() {
    FILE *cmd_fp;

    if ((cmd_fp = popen("sensors | grep -oE 'Package id [0-9]: * \\+[0-9]+\\.[0-9]+' | grep -oE '\\+[0-9]+\\.[0-9]+'", "r")) == NULL) {
        printf("ERROR ==> Could not read CPU temperature\n");
        return NULL;
    }

    char temp[16];
    double *ret = calloc(NUM_CPUS, sizeof(double));
    int i = 0;

    while(fgets(temp, sizeof(temp), cmd_fp) != NULL || i == NUM_CPUS) {
        ret[i++] = atof(temp);
    }

    pclose(cmd_fp);

    return ret;
}

// waits for the system to return to the base temperature
// returns the total cooldown time
int await_cooldown(const double* base_temps, double range, int delta) {
    int cooldown_time = 0;
    int cooled = 0;
    
    while (cooled != NUM_COOLDOWN) {
        sleep(delta);
        cooldown_time += delta;
        double *curr_temps = cpu_temp();

        int in_range = 0;
        for (int i = 0; i < NUM_CPUS; i++) {
            if (base_temps[i] + range >= curr_temps[i]) {
                in_range += 1;
            }
        }

        char *log_str[512];
        // TODO: generalize this 
        // sprintf(log_str, "TEMPS ==> CURR: %f %f | BASE: %f %f\n", curr_temps[0], curr_temps[1], base_temps[0], base_temps[1]);
        sprintf(log_str, "TEMPS ==> CURR: %f | BASE: %f \n", curr_temps[0], base_temps[0]);
        file_append(LOG_FILE, log_str);


        free(curr_temps);
        
        if (in_range == NUM_CPUS) {
            cooled++;
        } else {
            cooled = 0;
        }
    }

    return cooldown_time;
}

double* get_baseline(int num_samples, int delta) {

    double *avg_temps = calloc(NUM_CPUS, sizeof(double));

    for (int i = 1; i <= num_samples; i++) {
        sleep(delta);
        double *temp = cpu_temp();

        for (int j = 0; j < NUM_CPUS; j++) {
            avg_temps[j] = avg_temps[j] * (i - 1) / i + temp[j] / i;
        }

        free(temp);
    }

    return avg_temps;
}