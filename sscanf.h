#pragma once

typedef
	void (* logprintf_t)(char *, ...);

#define FLOAT_INFINITY          (0x7F800000)
#define FLOAT_NEG_INFINITY      (0xFF800000)
#define FLOAT_NAN               (0xFFFFFFFF)
#define FLOAT_NAN_E             (0x7FFFFFFF)
#define FLOAT_NEGATIVE_INFINITY (FLOAT_NEG_INFINITY)

