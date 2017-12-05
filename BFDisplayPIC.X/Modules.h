#define NAMELENGTH 12
#define VERSIONLENGTH 6
#define MAXMODULES 4

struct moduleInfo {
    char name[NAMELENGTH];
    char version_number[VERSIONLENGTH];
    char data1[20];
    char data2[20];
};
static struct moduleInfo modules[MAXMODULES];