#ifndef MASSERT_H
#define MASSERT_H
#pragma once
#define D_TRAP() {asm ("int $3");}
#ifndef __BUILD_DROP_ASSERT__
#define MASSERT(a) {if (a) {D_TRAP();}}
#else
#define MASSERT(a)
#endif

#endif // MASSERT_H
