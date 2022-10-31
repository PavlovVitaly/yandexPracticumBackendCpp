#ifndef CONFIG_H
#define CONFIG_H

struct Config {
	int min_edgewidth;
	int ignore_refresh;
	int max_edgecount; 
};

Config * ReadConfig (char * file);

#endif
