#ifndef MASSERT_H
#define MASSERT_H
#pragma once
#define D_TRAP() {asm ("int $3");}
#ifndef __BUILD_DROP_ASSERT__
#ifdef __BUILD_ASSERT_TRAP__
#define MASSERT(a) {if (a) {D_TRAP();}}
#else
#include <stdio.h>
#define MASSERT(a) {if (a) {puts("ASSERT\n");}}
#endif
#else
#define MASSERT(a)
#endif
/*may be move it ?*/
#define UNUSED(x) (void)(x)

#endif // MASSERT_H
