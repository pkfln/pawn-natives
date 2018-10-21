#pragma once

#include <stdexcept>
#include <amx/amx.h>

#if defined __cplusplus
	#define PAWN_NATIVE_EXTERN extern "C"
#else
	#define PAWN_NATIVE_EXTERN extern
#endif

#if defined _WIN32 || defined __CYGWIN__
	#define PAWN_NATIVE_DLLEXPORT __declspec(dllexport)
	#define PAWN_NATIVE_DLLIMPORT __declspec(dllimport)
	#define PAWN_NATIVE_API __cdecl
#elif defined __linux__ || defined __APPLE__
	#define PAWN_NATIVE_DLLEXPORT __attribute__((visibility("default")))
	#define PAWN_NATIVE_DLLIMPORT 
	#define PAWN_NATIVE_API __attribute__((cdecl))
#endif

#define PAWN_NATIVE_EXPORT PAWN_NATIVE_EXTERN PAWN_NATIVE_DLLEXPORT
#define PAWN_NATIVE_IMPORT PAWN_NATIVE_EXTERN PAWN_NATIVE_DLLIMPORT

#ifndef LOG_NATIVE_ERROR
	#define LOG_NATIVE_ERROR(...) ((void)0)
#endif
#ifndef LOG_NATIVE_WARNING
	#define LOG_NATIVE_WARNING(...) ((void)0)
#endif
#ifndef LOG_NATIVE_DEBUG
	#define LOG_NATIVE_DEBUG(...) ((void)0)
#endif
#ifndef LOG_NATIVE_INFO
	#define LOG_NATIVE_INFO(...) ((void)0)
#endif

namespace pawn_natives
{
	template <typename T>
	class TypeResolver
	{
	};

	template <typename T>
	class TypeResolver<void(T)>
	{
	public:
		// We need a specialisation because you can't just pass a type with a
		// parameter name directly as a typename, so we have to pass it as a
		// function type with no return and a parameter.  This is used to
		// (potentially) rename an already named parameter.  This is because
		// extracting the type is much simpler than extracting the name.
		typedef T type;
	};

	typedef struct varargs_s
	{
		explicit varargs_s(int num)
		:
			Count(num),
			Params((cell **)malloc(Count * sizeof (cell *)))
		{
			// This is used as "..." - instead of passing actual varargs, which
			// could be complex.
			if (!Params)
				throw std::bad_alloc();
		}

		varargs_s(AMX * amx, cell * params, int idx)
		:
			Count((int)params[0] / sizeof (cell) - idx + 1),
			Params((cell **)malloc(Count * sizeof (cell *)))
		{
			// This is used as "..." - instead of passing actual varargs, which
			// could be complex.
			if (!Params)
				throw std::bad_alloc();
			for (int i = 0; i != Count; ++i)
			{
				amx_GetAddr(amx, params[idx + i], Params + i);
			}
		}

		~varargs_s()
		{
			free(Params);
		}

		int     const Count;
		cell ** const Params;
	} * varargs_t;
};

#define PAWN_NATIVE__TYPE(tt) typename ::pawn_natives::TypeResolver<void(tt)>::type

#define PAWN_NATIVE__WITHOUT_PARAMS_int(...)   int
#define PAWN_NATIVE__WITHOUT_PARAMS_float(...) float
#define PAWN_NATIVE__WITHOUT_PARAMS_bool(...)  bool
#define PAWN_NATIVE__WITHOUT_PARAMS_void(...)  void
#define PAWN_NATIVE__WITHOUT_PARAMS_cell(...)  cell

#define PAWN_NATIVE__WITHOUT_RETURN_int(...)   __VA_ARGS__
#define PAWN_NATIVE__WITHOUT_RETURN_float(...) __VA_ARGS__
#define PAWN_NATIVE__WITHOUT_RETURN_bool(...)  __VA_ARGS__
#define PAWN_NATIVE__WITHOUT_RETURN_void(...)  __VA_ARGS__
#define PAWN_NATIVE__WITHOUT_RETURN_cell(...)  __VA_ARGS__

#define PAWN_NATIVE__MAYBE_RETURN_int(...)   return
#define PAWN_NATIVE__MAYBE_RETURN_float(...) return
#define PAWN_NATIVE__MAYBE_RETURN_bool(...)  return
#define PAWN_NATIVE__MAYBE_RETURN_void(...)  
#define PAWN_NATIVE__MAYBE_RETURN_cell(...)  return

#ifdef _MSC_VER
	#define PAWN_NATIVE__NUM_ARGS(...)  PAWN_NATIVE__NUM_ARGS_(PAWN_NATIVE__NUM_ARGS_MSVC(__VA_ARGS__))

	#define PAWN_NATIVE__NUM_ARGS_MSVC(...) unused, __VA_ARGS__
	#define PAWN_NATIVE__NUM_ARGS_(...) EXPAND(PAWN_NATIVE__NUM_ARGS_IMPL(__VA_ARGS__, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
	#define PAWN_NATIVE__NUM_ARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, N, ...) N
#else
	#define PAWN_NATIVE__NUM_ARGS(...) PAWN_NATIVE__NUM_ARGS_(__VA_ARGS__)
	#define PAWN_NATIVE__NUM_ARGS_(...) PAWN_NATIVE__NUM_ARGS_IMPL(0,##__VA_ARGS__, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
	#define PAWN_NATIVE__NUM_ARGS_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, N, ...) N
#endif

#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
#define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

#define EMPTY()
#define DEFER(id) id EMPTY()
#define OBSTRUCT(...) __VA_ARGS__ DEFER(EMPTY)()
#define EXPAND(...) __VA_ARGS__

#define EVAL(...)  EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL1(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL2(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL3(...) EVAL4(EVAL4(EVAL4(__VA_ARGS__)))
#define EVAL4(...) EVAL5(EVAL5(EVAL5(__VA_ARGS__)))
#define EVAL5(...) __VA_ARGS__

#define PAWN_NATIVE__INPUT_0() 
#define PAWN_NATIVE__INPUT_1(a)      PAWN_NATIVE__TYPE(a) p0
#define PAWN_NATIVE__INPUT_2(a,...)  PAWN_NATIVE__TYPE(a) p1 , EVAL(PAWN_NATIVE__INPUT_1 (__VA_ARGS__))
#define PAWN_NATIVE__INPUT_3(a,...)  PAWN_NATIVE__TYPE(a) p2 , EVAL(PAWN_NATIVE__INPUT_2 (__VA_ARGS__))
#define PAWN_NATIVE__INPUT_4(a,...)  PAWN_NATIVE__TYPE(a) p3 , EVAL(PAWN_NATIVE__INPUT_3 (__VA_ARGS__))
#define PAWN_NATIVE__INPUT_5(a,...)  PAWN_NATIVE__TYPE(a) p4 , EVAL(PAWN_NATIVE__INPUT_4 (__VA_ARGS__))
#define PAWN_NATIVE__INPUT_6(a,...)  PAWN_NATIVE__TYPE(a) p5 , EVAL(PAWN_NATIVE__INPUT_5 (__VA_ARGS__))
#define PAWN_NATIVE__INPUT_7(a,...)  PAWN_NATIVE__TYPE(a) p6 , EVAL(PAWN_NATIVE__INPUT_6 (__VA_ARGS__))
#define PAWN_NATIVE__INPUT_8(a,...)  PAWN_NATIVE__TYPE(a) p7 , EVAL(PAWN_NATIVE__INPUT_7 (__VA_ARGS__))
#define PAWN_NATIVE__INPUT_9(a,...)  PAWN_NATIVE__TYPE(a) p8 , EVAL(PAWN_NATIVE__INPUT_8 (__VA_ARGS__))
#define PAWN_NATIVE__INPUT_10(a,...) PAWN_NATIVE__TYPE(a) p9 , EVAL(PAWN_NATIVE__INPUT_9 (__VA_ARGS__))
#define PAWN_NATIVE__INPUT_11(a,...) PAWN_NATIVE__TYPE(a) p10, EVAL(PAWN_NATIVE__INPUT_10(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_12(a,...) PAWN_NATIVE__TYPE(a) p11, EVAL(PAWN_NATIVE__INPUT_11(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_13(a,...) PAWN_NATIVE__TYPE(a) p12, EVAL(PAWN_NATIVE__INPUT_12(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_14(a,...) PAWN_NATIVE__TYPE(a) p13, EVAL(PAWN_NATIVE__INPUT_13(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_15(a,...) PAWN_NATIVE__TYPE(a) p14, EVAL(PAWN_NATIVE__INPUT_14(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_16(a,...) PAWN_NATIVE__TYPE(a) p15, EVAL(PAWN_NATIVE__INPUT_15(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_17(a,...) PAWN_NATIVE__TYPE(a) p16, EVAL(PAWN_NATIVE__INPUT_16(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_18(a,...) PAWN_NATIVE__TYPE(a) p17, EVAL(PAWN_NATIVE__INPUT_17(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_19(a,...) PAWN_NATIVE__TYPE(a) p18, EVAL(PAWN_NATIVE__INPUT_18(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_20(a,...) PAWN_NATIVE__TYPE(a) p19, EVAL(PAWN_NATIVE__INPUT_19(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_21(a,...) PAWN_NATIVE__TYPE(a) p20, EVAL(PAWN_NATIVE__INPUT_20(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_22(a,...) PAWN_NATIVE__TYPE(a) p21, EVAL(PAWN_NATIVE__INPUT_21(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_23(a,...) PAWN_NATIVE__TYPE(a) p22, EVAL(PAWN_NATIVE__INPUT_22(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_24(a,...) PAWN_NATIVE__TYPE(a) p23, EVAL(PAWN_NATIVE__INPUT_23(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_25(a,...) PAWN_NATIVE__TYPE(a) p24, EVAL(PAWN_NATIVE__INPUT_24(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_26(a,...) PAWN_NATIVE__TYPE(a) p25, EVAL(PAWN_NATIVE__INPUT_25(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_27(a,...) PAWN_NATIVE__TYPE(a) p26, EVAL(PAWN_NATIVE__INPUT_26(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_28(a,...) PAWN_NATIVE__TYPE(a) p27, EVAL(PAWN_NATIVE__INPUT_27(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_29(a,...) PAWN_NATIVE__TYPE(a) p28, EVAL(PAWN_NATIVE__INPUT_28(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_30(a,...) PAWN_NATIVE__TYPE(a) p29, EVAL(PAWN_NATIVE__INPUT_29(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_31(a,...) PAWN_NATIVE__TYPE(a) p30, EVAL(PAWN_NATIVE__INPUT_30(__VA_ARGS__))
#define PAWN_NATIVE__INPUT_32(a,...) PAWN_NATIVE__TYPE(a) p31, EVAL(PAWN_NATIVE__INPUT_31(__VA_ARGS__))

#define PAWN_NATIVE__OUTPUT_0() 
#define PAWN_NATIVE__OUTPUT_1(a)      p0
#define PAWN_NATIVE__OUTPUT_2(a,...)  p1 , EVAL(PAWN_NATIVE__OUTPUT_1 (__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_3(a,...)  p2 , EVAL(PAWN_NATIVE__OUTPUT_2 (__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_4(a,...)  p3 , EVAL(PAWN_NATIVE__OUTPUT_3 (__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_5(a,...)  p4 , EVAL(PAWN_NATIVE__OUTPUT_4 (__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_6(a,...)  p5 , EVAL(PAWN_NATIVE__OUTPUT_5 (__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_7(a,...)  p6 , EVAL(PAWN_NATIVE__OUTPUT_6 (__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_8(a,...)  p7 , EVAL(PAWN_NATIVE__OUTPUT_7 (__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_9(a,...)  p8 , EVAL(PAWN_NATIVE__OUTPUT_8 (__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_10(a,...) p9 , EVAL(PAWN_NATIVE__OUTPUT_9 (__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_11(a,...) p10, EVAL(PAWN_NATIVE__OUTPUT_10(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_12(a,...) p11, EVAL(PAWN_NATIVE__OUTPUT_11(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_13(a,...) p12, EVAL(PAWN_NATIVE__OUTPUT_12(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_14(a,...) p13, EVAL(PAWN_NATIVE__OUTPUT_13(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_15(a,...) p14, EVAL(PAWN_NATIVE__OUTPUT_14(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_16(a,...) p15, EVAL(PAWN_NATIVE__OUTPUT_15(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_17(a,...) p16, EVAL(PAWN_NATIVE__OUTPUT_16(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_18(a,...) p17, EVAL(PAWN_NATIVE__OUTPUT_17(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_19(a,...) p18, EVAL(PAWN_NATIVE__OUTPUT_18(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_20(a,...) p19, EVAL(PAWN_NATIVE__OUTPUT_19(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_21(a,...) p20, EVAL(PAWN_NATIVE__OUTPUT_20(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_22(a,...) p21, EVAL(PAWN_NATIVE__OUTPUT_21(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_23(a,...) p22, EVAL(PAWN_NATIVE__OUTPUT_22(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_24(a,...) p23, EVAL(PAWN_NATIVE__OUTPUT_23(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_25(a,...) p24, EVAL(PAWN_NATIVE__OUTPUT_24(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_26(a,...) p25, EVAL(PAWN_NATIVE__OUTPUT_25(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_27(a,...) p26, EVAL(PAWN_NATIVE__OUTPUT_26(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_28(a,...) p27, EVAL(PAWN_NATIVE__OUTPUT_27(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_29(a,...) p28, EVAL(PAWN_NATIVE__OUTPUT_28(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_30(a,...) p29, EVAL(PAWN_NATIVE__OUTPUT_29(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_31(a,...) p30, EVAL(PAWN_NATIVE__OUTPUT_30(__VA_ARGS__))
#define PAWN_NATIVE__OUTPUT_32(a,...) p31, EVAL(PAWN_NATIVE__OUTPUT_31(__VA_ARGS__))

#define PAWN_NATIVE__INPUT_IMPL(...) CAT(PAWN_NATIVE__INPUT_, PAWN_NATIVE__NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)
#define PAWN_NATIVE__OUTPUT_IMPL(...) CAT(PAWN_NATIVE__OUTPUT_, PAWN_NATIVE__NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

#define PAWN_NATIVE__PARAMETERS(params) EXPAND(DEFER(PAWN_NATIVE__INPUT_IMPL)(CAT(PAWN_NATIVE__WITHOUT_RETURN_, params)))
#define PAWN_NATIVE__CALLING(params) EXPAND(DEFER(PAWN_NATIVE__OUTPUT_IMPL)(CAT(PAWN_NATIVE__WITHOUT_RETURN_, params)))

#define PAWN_NATIVE__RETURN(params) PAWN_NATIVE__WITHOUT_PARAMS_##params
#define PAWN_NATIVE__MAYBE_RETURN(params) PAWN_NATIVE__MAYBE_RETURN_##params

// Import a native from another plugin.
#define PAWN_IMPORT(nspace, func, type) PAWN_IMPORT_(nspace, func, type)
#define PAWN_IMPORT_(nspace, func, type) \
	PAWN_NATIVE_IMPORT PAWN_NATIVE__RETURN(type) PAWN_NATIVE_API                                       \
	    PAWN_NATIVE_##nspace##_##func(CAT(PAWN_NATIVE__WITHOUT_RETURN_, type));                        \
	                                                                                                   \
	namespace nspace                                                                                   \
	{                                                                                                  \
	    inline PAWN_NATIVE__RETURN(type)                                                               \
	        func(PAWN_NATIVE__PARAMETERS(type))                                                        \
	    {                                                                                              \
	        PAWN_NATIVE__MAYBE_RETURN(type)                                                            \
	            PAWN_NATIVE_##nspace##_##func(PAWN_NATIVE__CALLING(type));                             \
	    }                                                                                              \
	}


