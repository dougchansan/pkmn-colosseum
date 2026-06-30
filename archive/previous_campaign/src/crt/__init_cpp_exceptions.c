/*
 * MetroWerks CodeWarrior C++ exception initialization for GameCube.
 *
 * This file provides the .dtors entry that ensures global destructors
 * are called at program exit. The __destroy_global_chain_reference
 * is placed in the .dtors section and points to __destroy_global_chain,
 * which is called by the runtime during static destruction.
 */

extern void __destroy_global_chain(void);
extern void PPCHalt(void);
typedef void (*FuncPtr)(void);
extern FuncPtr _ctors[];

/*
 * This pointer is placed in the .dtors section. The CRT iterates
 * through .dtors entries and calls each function pointer during
 * program shutdown, ensuring all global C++ destructors are invoked.
 *
 * The MetroWerks GCN compiler uses __declspec(section) with the
 * section name to place data in the specified section. The pragma
 * is not needed for this version of the compiler.
 */
#pragma section ".dtors$10"
__declspec(section ".dtors$10") void (*const __destroy_global_chain_reference)(void) =
    __destroy_global_chain;

#if 0
asm void __init_cpp(void) {
#include "src/crt/__init_cpp_exceptions___init_cpp.inc"
}
#else
void __init_cpp(void) {
    FuncPtr* p = _ctors;
    while (*p != 0) {
        (*p)();
        p++;
    }
}
#endif
#if 0
asm void _ExitProcess(void) {
#include "src/crt/__init_cpp_exceptions__ExitProcess.inc"
}
#else
void _ExitProcess(void) {
    PPCHalt();
}
#endif
