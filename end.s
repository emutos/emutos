| ===========================================================================
| ==== end.s - Emulator specific startup module.  To be linked in last!
| ===========================================================================
|
| Copyright (c) 2001 Martin Doering.
|
| Authors:
|  MAD  Martin Doering
|
| This file is distributed under the GPL, version 2 or at your
| option any later version.  See doc/license.txt for details.
|



	.bss
	.globl	_bssend
_bssend:
	.ds.w	1
| ===========================================================================
| ==== End ==================================================================
| ===========================================================================

	.end

