#ifndef AWGET_H
#define AWGET_H

#include <string>

struct chainlist_packet {
	unsigned short chainlistLength, urlLength, numberChainlist;
	char* chainlist, url;
};

#endif
