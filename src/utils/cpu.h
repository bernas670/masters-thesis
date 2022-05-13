
double* cpu_temp();
int await_cooldown(const double* base_temps, double range, int delta);
double* get_baseline(int num_samples, int delta);