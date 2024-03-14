/*
 * elfalias.ld - Linker script fragment for ELF leading-underscore issues
 *
 * Copyright (C) 2023-2024 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef ELF_TOOLCHAIN
#error This file is intended for ELF toolchain only.
#endif

/* Our code is compiled with -fleading-underscore, so the references to external
 * libraries will include a leading underscore. However, libgcc was not compiled
 * with that option, so its symbols do not start with a leading underscore. So
 * we need to define leading-underscore aliases to the no-leading-underscore
 * functions in libgcc.
 */

/* Define an optional alias to a target symbol.
 * If the alias is not referenced, it won't be defined, so the target won't be
 * referenced either.
 * This can safely be used to reference library functions:
 * as usual, objects will only be pulled if required for the current build.
 */
#define DEFINE_OPTIONAL_SYMBOL_ALIAS(alias, target) PROVIDE(alias = target);

/* Create a leading-underscore alias for an ELF library function. */
#define ELF_LIB_REF(symbol) DEFINE_OPTIONAL_SYMBOL_ALIAS(_##symbol, symbol)

/* List of library functions that *may* be used by our program.
 * If we don't actually use them, our binary won't grow purposeless.
 */
ELF_LIB_REF(__mulsi3)
ELF_LIB_REF(__divsi3)
ELF_LIB_REF(__modsi3)
ELF_LIB_REF(__udivsi3)
ELF_LIB_REF(__umodsi3)

/* The aliases above may create new unresolved externals,
 * so we link the target libraries again.
 * That could also be achieved on the command line, after the linker script.
 */
INPUT(-lgcc)
