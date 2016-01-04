#ifndef OBJ_TEST_HPP
#define OBJ_TEST_HPP
#include <PalmTypes.h>
#ifdef TEST_OBJECT_UNIT
void perform_object_tests();
#endif

#ifdef BENCH_OBJECTS
void perform_benchmarks();
void add_reallocation(UInt32 size);
void reset_statistics();
#endif
#ifdef PERFORM_TEST

void perform_test();
#endif
#endif

