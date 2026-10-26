/* SPDX-License-Identifier: MIT
 * Minimal freestanding adapters, not a general C++ standard library.
 * State serialization and debug strings are unavailable on this target.
 * Core FM calculations remain unmodified. */
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
/* Supported instrument profile: five FM channels, permanently enabled DAC.
 * No MIDI/register API exposes channel 6 FM or toggling DAC during a song. */
#define EMERALD_FIXED_DAC 1
/* App mix gain after summing the five FM channels, before adding DAC. */
extern uint32_t emerald_fm_gain_q8;
#define assert(x) ((void)0)
void *operator new(size_t n);
void *operator new(size_t, void *p) noexcept;
void operator delete(void *p) noexcept;
void operator delete(void *p, size_t n) noexcept;
extern "C" void ym_unavailable(void);
namespace std {
template<class T> const T& min(const T&a,const T&b){return a<b?a:b;}
template<class T> const T& max(const T&a,const T&b){return a>b?a:b;}
template<class P,class N,class T> void fill_n(P p,N n,const T&v){while(n--)*p++=v;}
template<class T,size_t N> struct array {
 T values[N];size_t size()const{return N;}T&operator[](size_t i){return values[i];}const T&operator[](size_t i)const{return values[i];}
 T*begin(){return values;}T*end(){return values+N;}
};
template<class T> class unique_ptr {
 T*p;
public:
 unique_ptr():p(nullptr){} explicit unique_ptr(T*q):p(q){}
 unique_ptr(unique_ptr&&q):p(q.p){q.p=nullptr;}
 unique_ptr&operator=(unique_ptr&&q){p=q.p;q.p=nullptr;return *this;}
 T*get()const{return p;}T*operator->()const{return p;}T&operator*()const{return *p;}
 explicit operator bool()const{return p!=nullptr;}
};
template<class T,class... A> unique_ptr<T> make_unique(A&&... a){return unique_ptr<T>(new T(static_cast<A&&>(a)...));}
template<class T> struct vector {
 void resize(size_t){ym_unavailable();}void push_back(T){ym_unavailable();}size_t size()const{return 0;}bool empty()const{return true;}
 T&operator[](size_t){ym_unavailable();static T x;return x;}
};
struct string {string(const char*){} const char*c_str()const{return "";}};
}
