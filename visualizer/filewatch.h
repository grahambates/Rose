
#include <string>

#include <sys/stat.h>
#include <time.h>

class FileWatch {
	std::string filename;
	struct stat filestat = {};
	bool valid;
public:
	FileWatch(const char *name): filename(name) {
		valid = stat(filename.c_str(), &filestat) == 0;
	}

	bool changed() {
		struct stat newfilestat;
		if (stat(filename.c_str(), &newfilestat) != 0) return false;
		if (!valid || newfilestat.st_mtime != filestat.st_mtime) {
			filestat = newfilestat;
			valid = true;
			return true;
		} else {
			return false;
		}
	}

	const char *name() {
		return filename.c_str();
	}

	const char *time_text() {
		return ctime(&filestat.st_mtime);
	}
};
