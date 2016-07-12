#ifndef SWITCHMACRO_H_
#define SWITCHMACRO_H_

//	two algorithm
#define PHONGTRACE
//#define PATHTRACE

//	three ways of find nearest intersection
#define KD
//#define REGULAR
//#define PUSU

//	if choose REGULAR way
//	two sizes of world
#ifdef REGULAR
#define BIGWORLD
//#define SMALLWORLD
#endif

//	if choose ray tracing
//	if texture or not
//	if aperture or not
//	if important sample or not
//	if supersample or not
#ifdef PHONGTRACE
#define TEXTURE
//#define APERTURE
//#define SAMPLEWAY
#define SUPERSAMPLE
#endif

//	if choose path tracing
//	the 3 material types of object
#ifdef PATHTRACE
#define DIFF 0
#define SPEC 1
#define REFR 2
#endif

//	the type of object
#define SPHERE 0
#define LIGHT 1
#define PLANE 2
#define TRIANGLE 3

//	multi thread or not
#define THREAD
#endif