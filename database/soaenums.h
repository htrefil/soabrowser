#ifndef soaenums_h
#define soaenums_h

#define DB_EXTENSION	".db"
#define ID_COL_INDEX	0
#define XML_COL_INDEX	1
#define DES_COL_INDEX	2



enum SoaClType { sctInv, sctUid, sctInt, sctTxt, sctXml, sctRow };

enum SoaClNameType { scnInvalid = 0, scnDatabase = 1, scnMerge = 2, scnSchema = 4 };

enum SoaPlType { sptMySQL, sptSQLServer, sptSQLite };

#endif
