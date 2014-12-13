#ifndef MASSERT_H
#define MASSERT_H
#pragma once
#ifndef __BUILD_DROP_ASSERT__
#define MASSERT(a) {if (a) {asm ("int $3");}}
#else
#define MASSERT(a)
#endif

#endif // MASSERT_H
