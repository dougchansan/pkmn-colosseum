typedef void (*DtorFunc)(void);

extern void __destroy_global_chain(void);

__declspec(section ".dtors") DtorFunc const __destroy_global_chain_reference =
    __destroy_global_chain;
