#pragma once

#include <filesystem>
#include <MinHook.h>
#include <Windows.h>
#include <assert.h>
#include <Psapi.h>
#include <ostream>
#include <fstream>
#include <random>
#include <vector>
#include <string>

#include <type_traits>

template <typename F> auto IIFE(F f) { return f(); }

template <class T> struct remove_cvref { typedef std::remove_cv_t<std::remove_reference_t<T>> type; };

template <class T> using remove_cvref_t = typename remove_cvref<T>::type;

template <typename Ret, typename Type> Ret& direct_access(Type* type, size_t offset) {
	union {
		size_t raw;
		Type* source;
		Ret* target;
	} u;
	u.source = type;
	u.raw += offset;
	return *u.target;
}

#define AS_FIELD(type, name, fn) __declspec(property(get = fn, put = set##name)) type name
#define DEF_FIELD_RW(type, name) __declspec(property(get = get##name, put = set##name)) type name

#define FAKE_FIELD(type, name)                                                                                       \
AS_FIELD(type, name, get##name);                                                                                     \
type get##name()

#define BUILD_ACCESS(type, name, offset)                                                                             \
AS_FIELD(type, name, get##name);                                                                                     \
type get##name() const { return direct_access<type>(this, offset); }												 \
void set##name(type v) const { direct_access<type>(this, offset) = v; }
namespace mem {
extern HMODULE g_hModule;
extern uintptr_t mod;
extern bool isRunning;

	void writeBytes(uintptr_t location, BYTE* bytes, size_t amountOfBytes);
	//Will write the bytes in "bytes", be sure that there is enough room.
	void readBytes(uintptr_t location, BYTE* bytes, size_t amountOfBytess);

	
	//Original bytes can be null if you don't need them
	void nop(uintptr_t location, size_t amountOfBytes, BYTE* OriginalBytes = NULL);

	//Use this to find paterns in memory, you can use ??
	uintptr_t FindSignature(const char* Signature);
	uintptr_t FindSignatureInModule(const char* modName, const char* szSignature);

	///Use this if you want to resolve a pointer
	///will return 0 if a part of the path is null
	uintptr_t ResolvePointer(uint16_t* offsets, uint8_t offsetCount);

	//will return the vtable of an object
	uintptr_t** getVtable(void* object);
	//will set the vtable of an object
	void ApplyVtable(void* object, uintptr_t** Vtable);
}