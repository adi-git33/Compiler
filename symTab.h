typedef enum {None = 0, Integer, String, Set, Collection} varType;

struct {
	char *name;
	varType type;
} SymTable[100];