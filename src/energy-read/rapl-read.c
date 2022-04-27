/* Read the RAPL registers on recent (>sandybridge) Intel processors	*/
/*									*/
/* There are currently three ways to do this:				*/
/*	1. Read the MSRs directly with /dev/cpu/??/msr			*/
/*	2. Use the perf_event_open() interface				*/
/*	3. Read the values from the sysfs powercap interface		*/
/*									*/
/* MSR Code originally based on a (never made it upstream) linux-kernel	*/
/*	RAPL driver by Zhang Rui <rui.zhang@intel.com>			*/
/*	https://lkml.org/lkml/2011/5/26/93				*/
/* Additional contributions by:						*/
/*	Romain Dolbeau -- romain @ dolbeau.org				*/
/*									*/
/* For raw MSR access the /dev/cpu/??/msr driver must be enabled and	*/
/*	permissions set to allow read access.				*/
/*	You might need to "modprobe msr" before it will work.		*/
/*									*/
/* perf_event_open() support requires at least Linux 3.14 and to have	*/
/*	/proc/sys/kernel/perf_event_paranoid < 1			*/
/*									*/
/* the sysfs powercap interface got into the kernel in 			*/
/*	2d281d8196e38dd (3.13)						*/
/*									*/
/* Compile with:   gcc -O2 -Wall -o rapl-read rapl-read.c -lm		*/
/*									*/
/* Vince Weaver -- vincent.weaver @ maine.edu -- 11 September 2015	*/
/*									*/

#include "rapl-read.h"
#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#include <sys/syscall.h>
#include <linux/perf_event.h>

#define MSR_RAPL_POWER_UNIT		0x606

/*
 * Platform specific RAPL Domains.
 * Note that PP1 RAPL Domain is supported on 062A only
 * And DRAM RAPL Domain is supported on 062D only
 */
/* Package RAPL Domain */
#define MSR_PKG_RAPL_POWER_LIMIT	0x610
#define MSR_PKG_ENERGY_STATUS		0x611
#define MSR_PKG_PERF_STATUS		0x613
#define MSR_PKG_POWER_INFO		0x614

/* PP0 RAPL Domain */
#define MSR_PP0_POWER_LIMIT		0x638
#define MSR_PP0_ENERGY_STATUS		0x639
#define MSR_PP0_POLICY			0x63A
#define MSR_PP0_PERF_STATUS		0x63B

/* PP1 RAPL Domain, may reflect to uncore devices */
#define MSR_PP1_POWER_LIMIT		0x640
#define MSR_PP1_ENERGY_STATUS		0x641
#define MSR_PP1_POLICY			0x642

/* DRAM RAPL Domain */
#define MSR_DRAM_POWER_LIMIT		0x618
#define MSR_DRAM_ENERGY_STATUS		0x619
#define MSR_DRAM_PERF_STATUS		0x61B
#define MSR_DRAM_POWER_INFO		0x61C

/* PSYS RAPL Domain */
#define MSR_PLATFORM_ENERGY_STATUS	0x64d

/* RAPL UNIT BITMASK */
#define POWER_UNIT_OFFSET	0
#define POWER_UNIT_MASK		0x0F

#define ENERGY_UNIT_OFFSET	0x08
#define ENERGY_UNIT_MASK	0x1F00

#define TIME_UNIT_OFFSET	0x10
#define TIME_UNIT_MASK		0xF000

static int open_msr(int core) {

	char msr_filename[BUFSIZ];
	int fd;

	sprintf(msr_filename, "/dev/cpu/%d/msr", core);
	fd = open(msr_filename, O_RDONLY);
	if ( fd < 0 ) {
		if ( errno == ENXIO ) {
			fprintf(stderr, "rdmsr: No CPU %d\n", core);
			exit(2);
		} else if ( errno == EIO ) {
			fprintf(stderr, "rdmsr: CPU %d doesn't support MSRs\n",
					core);
			exit(3);
		} else {
			perror("rdmsr:open");
			fprintf(stderr,"Trying to open %s\n",msr_filename);
			exit(127);
		}
	}

	return fd;
}

static long long read_msr(int fd, int which) {

	uint64_t data;

	if ( pread(fd, &data, sizeof data, which) != sizeof data ) {
		perror("rdmsr:pread");
		exit(127);
	}

	return (long long)data;
}

#define CPU_SANDYBRIDGE		42
#define CPU_SANDYBRIDGE_EP	45
#define CPU_IVYBRIDGE		58
#define CPU_IVYBRIDGE_EP	62
#define CPU_HASWELL		60
#define CPU_HASWELL_ULT		69
#define CPU_HASWELL_GT3E	70
#define CPU_HASWELL_EP		63
#define CPU_BROADWELL		61
#define CPU_BROADWELL_GT3E	71
#define CPU_BROADWELL_EP	79
#define CPU_BROADWELL_DE	86
#define CPU_SKYLAKE		78
#define CPU_SKYLAKE_HS		94
#define CPU_SKYLAKE_X		85
#define CPU_KNIGHTS_LANDING	87
#define CPU_KNIGHTS_MILL	133
#define CPU_KABYLAKE_MOBILE	142
#define CPU_KABYLAKE		158
#define CPU_ATOM_SILVERMONT	55
#define CPU_ATOM_AIRMONT	76
#define CPU_ATOM_MERRIFIELD	74
#define CPU_ATOM_MOOREFIELD	90
#define CPU_ATOM_GOLDMONT	92
#define CPU_ATOM_GEMINI_LAKE	122
#define CPU_ATOM_DENVERTON	95

/* TODO: on Skylake, also may support  PSys "platform" domain,	*/
/* the whole SoC not just the package.				*/
/* see dcee75b3b7f025cc6765e6c92ba0a4e59a4d25f4			*/

static int detect_cpu(void) {

	FILE *fff;

	int family,model=-1;
	char buffer[BUFSIZ],*result;
	char vendor[BUFSIZ];

	fff=fopen("/proc/cpuinfo","r");
	if (fff==NULL) return -1;

	while(1) {
		result=fgets(buffer,BUFSIZ,fff);
		if (result==NULL) break;

		if (!strncmp(result,"vendor_id",8)) {
			sscanf(result,"%*s%*s%s",vendor);

			if (strncmp(vendor,"GenuineIntel",12)) {
				printf("%s not an Intel chip\n",vendor);
				return -1;
			}
		}

		if (!strncmp(result,"cpu family",10)) {
			sscanf(result,"%*s%*s%*s%d",&family);
			if (family!=6) {
				printf("Wrong CPU family %d\n",family);
				return -1;
			}
		}

		if (!strncmp(result,"model",5)) {
			sscanf(result,"%*s%*s%d",&model);
		}

	}

	fclose(fff);

	printf("Found ");

	switch(model) {
		case CPU_SANDYBRIDGE:
			printf("Sandybridge");
			break;
		case CPU_SANDYBRIDGE_EP:
			printf("Sandybridge-EP");
			break;
		case CPU_IVYBRIDGE:
			printf("Ivybridge");
			break;
		case CPU_IVYBRIDGE_EP:
			printf("Ivybridge-EP");
			break;
		case CPU_HASWELL:
		case CPU_HASWELL_ULT:
		case CPU_HASWELL_GT3E:
			printf("Haswell");
			break;
		case CPU_HASWELL_EP:
			printf("Haswell-EP");
			break;
		case CPU_BROADWELL:
		case CPU_BROADWELL_GT3E:
			printf("Broadwell");
			break;
		case CPU_BROADWELL_EP:
			printf("Broadwell-EP");
			break;
		case CPU_SKYLAKE:
		case CPU_SKYLAKE_HS:
			printf("Skylake");
			break;
		case CPU_SKYLAKE_X:
			printf("Skylake-X");
			break;
		case CPU_KABYLAKE:
		case CPU_KABYLAKE_MOBILE:
			printf("Kaby Lake");
			break;
		case CPU_KNIGHTS_LANDING:
			printf("Knight's Landing");
			break;
		case CPU_KNIGHTS_MILL:
			printf("Knight's Mill");
			break;
		case CPU_ATOM_GOLDMONT:
		case CPU_ATOM_GEMINI_LAKE:
		case CPU_ATOM_DENVERTON:
			printf("Atom");
			break;
		default:
			printf("Unsupported model %d\n",model);
			model=-1;
			break;
	}

	printf(" Processor type\n");

	return model;
}

#define MAX_CPUS	1024
#define MAX_PACKAGES	16

static int total_cores=0,total_packages=0;
static int package_map[MAX_PACKAGES];

static int detect_packages(void) {

	char filename[BUFSIZ];
	FILE *fff;
	int package;
	int i;

	for(i=0;i<MAX_PACKAGES;i++) package_map[i]=-1;

	printf("\t");
	for(i=0;i<MAX_CPUS;i++) {
		sprintf(filename,"/sys/devices/system/cpu/cpu%d/topology/physical_package_id",i);
		fff=fopen(filename,"r");
		if (fff==NULL) break;
		fscanf(fff,"%d",&package);
		printf("%d (%d)",i,package);
		if (i%8==7) printf("\n\t"); else printf(", ");
		fclose(fff);

		if (package_map[package]==-1) {
			total_packages++;
			package_map[package]=i;
		}

	}

	printf("\n");

	total_cores=i;

	printf("\tDetected %d cores in %d packages\n\n",
		total_cores,total_packages);

	return 0;
}


/*******************************/
/* MSR code                    */
/*******************************/
static pkg_arr rapl_msr(int core, int cpu_model, char *cmd) {

	int fd;
	long long result;
	double power_units,time_units;
	double cpu_energy_units[MAX_PACKAGES],dram_energy_units[MAX_PACKAGES];
	double package_before[MAX_PACKAGES],package_after[MAX_PACKAGES];
	double pp0_before[MAX_PACKAGES],pp0_after[MAX_PACKAGES];
	double pp1_before[MAX_PACKAGES],pp1_after[MAX_PACKAGES];
	double dram_before[MAX_PACKAGES],dram_after[MAX_PACKAGES];
	double psys_before[MAX_PACKAGES],psys_after[MAX_PACKAGES];
	double thermal_spec_power,minimum_power,maximum_power,time_window;
	int j;

	int dram_avail=0,pp0_avail=0,pp1_avail=0,psys_avail=0;
	int different_units=0;

	pkg_arr ret = {
		.data = NULL,
		.size = -1
	};

	printf("\nTrying /dev/msr interface to gather results\n\n");

	if (cpu_model<0) {
		printf("\tUnsupported CPU model %d\n",cpu_model);
		return ret;
	}

	switch(cpu_model) {

		case CPU_SANDYBRIDGE_EP:
		case CPU_IVYBRIDGE_EP:
			pp0_avail=1;
			pp1_avail=0;
			dram_avail=1;
			different_units=0;
			psys_avail=0;
			break;

		case CPU_HASWELL_EP:
		case CPU_BROADWELL_EP:
		case CPU_SKYLAKE_X:
			pp0_avail=1;
			pp1_avail=0;
			dram_avail=1;
			different_units=1;
			psys_avail=0;
			break;

		case CPU_KNIGHTS_LANDING:
		case CPU_KNIGHTS_MILL:
			pp0_avail=0;
			pp1_avail=0;
			dram_avail=1;
			different_units=1;
			psys_avail=0;
			break;

		case CPU_SANDYBRIDGE:
		case CPU_IVYBRIDGE:
			pp0_avail=1;
			pp1_avail=1;
			dram_avail=0;
			different_units=0;
			psys_avail=0;
			break;

		case CPU_HASWELL:
		case CPU_HASWELL_ULT:
		case CPU_HASWELL_GT3E:
		case CPU_BROADWELL:
		case CPU_BROADWELL_GT3E:
		case CPU_ATOM_GOLDMONT:
		case CPU_ATOM_GEMINI_LAKE:
		case CPU_ATOM_DENVERTON:
			pp0_avail=1;
			pp1_avail=1;
			dram_avail=1;
			different_units=0;
			psys_avail=0;
			break;

		case CPU_SKYLAKE:
		case CPU_SKYLAKE_HS:
		case CPU_KABYLAKE:
		case CPU_KABYLAKE_MOBILE:
			pp0_avail=1;
			pp1_avail=1;
			dram_avail=1;
			different_units=0;
			psys_avail=1;
			break;

	}

	for(j=0;j<total_packages;j++) {
		printf("\tListing paramaters for package #%d\n",j);

		fd=open_msr(package_map[j]);

		/* Calculate the units used */
		result=read_msr(fd,MSR_RAPL_POWER_UNIT);

		power_units=pow(0.5,(double)(result&0xf));
		cpu_energy_units[j]=pow(0.5,(double)((result>>8)&0x1f));
		time_units=pow(0.5,(double)((result>>16)&0xf));

		/* On Haswell EP and Knights Landing */
		/* The DRAM units differ from the CPU ones */
		if (different_units) {
			dram_energy_units[j]=pow(0.5,(double)16);
			printf("DRAM: Using %lf instead of %lf\n",
				dram_energy_units[j],cpu_energy_units[j]);
		}
		else {
			dram_energy_units[j]=cpu_energy_units[j];
		}

		printf("\t\tPower units = %.3fW\n",power_units);
		printf("\t\tCPU Energy units = %.8fJ\n",cpu_energy_units[j]);
		printf("\t\tDRAM Energy units = %.8fJ\n",dram_energy_units[j]);
		printf("\t\tTime units = %.8fs\n",time_units);
		printf("\n");

		/* Show package power info */
		result=read_msr(fd,MSR_PKG_POWER_INFO);
		thermal_spec_power=power_units*(double)(result&0x7fff);
		printf("\t\tPackage thermal spec: %.3fW\n",thermal_spec_power);
		minimum_power=power_units*(double)((result>>16)&0x7fff);
		printf("\t\tPackage minimum power: %.3fW\n",minimum_power);
		maximum_power=power_units*(double)((result>>32)&0x7fff);
		printf("\t\tPackage maximum power: %.3fW\n",maximum_power);
		time_window=time_units*(double)((result>>48)&0x7fff);
		printf("\t\tPackage maximum time window: %.6fs\n",time_window);

		/* Show package power limit */
		result=read_msr(fd,MSR_PKG_RAPL_POWER_LIMIT);
		printf("\t\tPackage power limits are %s\n", (result >> 63) ? "locked" : "unlocked");
		double pkg_power_limit_1 = power_units*(double)((result>>0)&0x7FFF);
		double pkg_time_window_1 = time_units*(double)((result>>17)&0x007F);
		printf("\t\tPackage power limit #1: %.3fW for %.6fs (%s, %s)\n",
			pkg_power_limit_1, pkg_time_window_1,
			(result & (1LL<<15)) ? "enabled" : "disabled",
			(result & (1LL<<16)) ? "clamped" : "not_clamped");
		double pkg_power_limit_2 = power_units*(double)((result>>32)&0x7FFF);
		double pkg_time_window_2 = time_units*(double)((result>>49)&0x007F);
		printf("\t\tPackage power limit #2: %.3fW for %.6fs (%s, %s)\n", 
			pkg_power_limit_2, pkg_time_window_2,
			(result & (1LL<<47)) ? "enabled" : "disabled",
			(result & (1LL<<48)) ? "clamped" : "not_clamped");

		/* only available on *Bridge-EP */
		if ((cpu_model==CPU_SANDYBRIDGE_EP) || (cpu_model==CPU_IVYBRIDGE_EP)) {
			result=read_msr(fd,MSR_PKG_PERF_STATUS);
			double acc_pkg_throttled_time=(double)result*time_units;
			printf("\tAccumulated Package Throttled Time : %.6fs\n",
				acc_pkg_throttled_time);
		}

		/* only available on *Bridge-EP */
		if ((cpu_model==CPU_SANDYBRIDGE_EP) || (cpu_model==CPU_IVYBRIDGE_EP)) {
			result=read_msr(fd,MSR_PP0_PERF_STATUS);
			double acc_pp0_throttled_time=(double)result*time_units;
			printf("\tPowerPlane0 (core) Accumulated Throttled Time "
				": %.6fs\n",acc_pp0_throttled_time);

			result=read_msr(fd,MSR_PP0_POLICY);
			int pp0_policy=(int)result&0x001f;
			printf("\tPowerPlane0 (core) for core %d policy: %d\n",core,pp0_policy);

		}


		if (pp1_avail) {
			result=read_msr(fd,MSR_PP1_POLICY);
			int pp1_policy=(int)result&0x001f;
			printf("\tPowerPlane1 (on-core GPU if avail) %d policy: %d\n",
				core,pp1_policy);
		}
		close(fd);

	}
	printf("\n");

	for(j=0;j<total_packages;j++) {

		fd=open_msr(package_map[j]);

		/* Package Energy */
		result=read_msr(fd,MSR_PKG_ENERGY_STATUS);
		package_before[j]=(double)result*cpu_energy_units[j];

		/* PP0 energy */
		/* Not available on Knights* */
		/* Always returns zero on Haswell-EP? */
		if (pp0_avail) {
			result=read_msr(fd,MSR_PP0_ENERGY_STATUS);
			pp0_before[j]=(double)result*cpu_energy_units[j];
		}

		/* PP1 energy */
		/* not available on *Bridge-EP */
		if (pp1_avail) {
	 		result=read_msr(fd,MSR_PP1_ENERGY_STATUS);
			pp1_before[j]=(double)result*cpu_energy_units[j];
		}


		/* Updated documentation (but not the Vol3B) says Haswell and	*/
		/* Broadwell have DRAM support too				*/
		if (dram_avail) {
			result=read_msr(fd,MSR_DRAM_ENERGY_STATUS);
			dram_before[j]=(double)result*dram_energy_units[j];
		}


		/* Skylake and newer for Psys				*/
		if ((cpu_model==CPU_SKYLAKE) ||
			(cpu_model==CPU_SKYLAKE_HS) ||
			(cpu_model==CPU_KABYLAKE) ||
			(cpu_model==CPU_KABYLAKE_MOBILE)) {

			result=read_msr(fd,MSR_PLATFORM_ENERGY_STATUS);
			psys_before[j]=(double)result*cpu_energy_units[j];
		}

		close(fd);
	}

	struct timeval stop, start;
	gettimeofday(&start, NULL);

	system(cmd);

	gettimeofday(&stop, NULL);
	ret.time = (double) ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec) / 1000.0f;

	printf("\n\n\t==> execution time : %f ms\n\n", ret.time);

	if ((ret.data = calloc(total_packages, sizeof(rapl_st*))) == NULL) {
		printf("\n\tCould not allocate memory\n\n");
		return ret;
	}
	ret.size = total_packages;

	
	for(j=0;j<total_packages;j++) {

		ret.data[j] = calloc(1, sizeof(rapl_st));

		fd=open_msr(package_map[j]);

		printf("\tPackage %d:\n",j);

		result=read_msr(fd,MSR_PKG_ENERGY_STATUS);
		package_after[j]=(double)result*cpu_energy_units[j];
		ret.data[j]->pkg_energy = package_after[j]-package_before[j];
		printf("\t\tPackage energy: %.6fJ\n", ret.data[j]->pkg_energy);

		result=read_msr(fd,MSR_PP0_ENERGY_STATUS);
		pp0_after[j]=(double)result*cpu_energy_units[j];
		ret.data[j]->pp0 = pp0_after[j]-pp0_before[j];
		printf("\t\tPowerPlane0 (cores): %.6fJ\n", ret.data[j]->pp0);

		/* not available on SandyBridge-EP */
		if (pp1_avail) {
			result=read_msr(fd,MSR_PP1_ENERGY_STATUS);
			pp1_after[j]=(double)result*cpu_energy_units[j];
			ret.data[j]->pp1 = pp1_after[j]-pp1_before[j];
			printf("\t\tPowerPlane1 (on-core GPU if avail): %.6f J\n", ret.data[j]->pp1);
		}

		if (dram_avail) {
			result=read_msr(fd,MSR_DRAM_ENERGY_STATUS);
			dram_after[j]=(double)result*dram_energy_units[j];
			ret.data[j]->dram = dram_after[j]-dram_before[j];
			printf("\t\tDRAM: %.6fJ\n", ret.data[j]->dram);
		}

		if (psys_avail) {
			result=read_msr(fd,MSR_PLATFORM_ENERGY_STATUS);
			psys_after[j]=(double)result*cpu_energy_units[j];
			ret.data[j]->psys = psys_after[j]-psys_before[j];
			printf("\t\tPSYS: %.6fJ\n", ret.data[j]->psys);
		}

		close(fd);
	}
	printf("\n");
	// printf("Note: the energy measurements can overflow in 60s or so\n");
	// printf("      so try to sample the counters more often than that.\n\n");

	return ret;
}




pkg_arr rapl_read(char* cmd) {
	int cpu_model;

	cpu_model = detect_cpu();
	detect_packages();

	return rapl_msr(0,cpu_model,cmd);
}