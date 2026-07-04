#ifndef CORE_FILESYS
#define CORE_FILESYS

class FileSystem {
private:
public:
	static bool get(const char *filename, unsigned char buffer[]);
};

#endif