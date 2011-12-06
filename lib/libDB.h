#ifndef __LIB_DB__
#define __LIB_DB__

typedef struct _DBObject {

	char name[32];
	uint32_t version;

	struct _DBObject* next;
} DBObject;

extern void DBFree();
extern DBObject* DBRead(const char* path);
extern void DBWrite(const char* path);
extern DBObject* DBFind(const char* name);
extern DBObject* DBPrepend(const char* name, uint32_t version);

#endif
