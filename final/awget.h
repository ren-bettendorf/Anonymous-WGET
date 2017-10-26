#ifndef AWGET_H
#define AWGET_H

#include <string>

struct chainlist_packet {
	unsigned short chainlistLength, urlLength, numberChainlist;
	const char* chainlist;
	const char* url;
};

#endif
