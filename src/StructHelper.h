#ifndef StructHelper_h
#define StructHelper_h

#define DECLARE_STRUCT_OPERATORS(STRUCT_TYPE) \
    inline bool operator==(STRUCT_TYPE a, STRUCT_TYPE b) { return memcmp(&a, &b, sizeof(STRUCT_TYPE)) == 0; }

#endif