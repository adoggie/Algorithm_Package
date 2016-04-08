


#ifndef ERICDONG_MATRIX_DEFINES
#define ERICDONG_MATRIX_DEFINES

#define M_IF_USE_TEMPFILE		FALSE

//#define MEMORY_SIZE (1024*1024*16)
#define MEMORY_SIZE (1024*2)

#define VECT_HAS_VALUE			1
#define VECT_NO_LONGER_USE		2
#define VECT_VALUE_MODIFIED		4
#define VECT_VALUE_IN_MEMORY	8

#define VECTOR_HEADER			8

#define MATRIX_RESERVE_BUF		100000000
#define MATRIX_CO_USED			2

#define TRANS_BUF_WIDTH			4096//128
#define TRANS_BUF_HEIGHT		32//128

//#define TRANS_BUF_WIDTH			32//小数据测试用
//#define TRANS_BUF_HEIGHT		32//小数据测试用


#endif