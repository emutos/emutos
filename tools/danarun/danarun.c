/*
 * danarun.c : PalmOS boot loader
 *
 * This program is a very simple and stupid PalmOS program which will load and
 * run the file "/kernel.img" from any VFS volume. The binary can be any size
 * (that will fit in memory). It's loaded in chunks to fit the PalmOS heap,
 * then reassembled into a contiguous image at an arbitrary address before the
 * system is switched to supervisor mode and the image is run.
 *
 * Compile with the supplied Makefile. You'll need prctools.
 *
 * Copyright (C) 2021 David Given
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include <PalmOS.h>
#include <VFSMgr.h>
#include <string.h>

static UInt8 bootloader[] = {
#include "../../obj/palmboot.h"
};

#define BLOCK_SIZE (32L*1024L)
#define KERNEL_SIZE (300L*1024L)

static UInt8* blocks[KERNEL_SIZE/BLOCK_SIZE];

static Int16 comparator_cb(void* p1, void* p2, Int32 other)
{
	void* pp1 = *(void**)p1;
	void* pp2 = *(void**)p2;

	if (pp1 < pp2)
		return -1;
	if (pp1 > pp2)
		return 1;
	return 0;
}

static void print(const char* s)
{
	EventType et;

	WinEraseWindow();
	WinDrawChars(s, strlen(s), 10, 10);

	for (;;)
	{
		EvtGetEvent(&et, 0);
	    SysHandleEvent(&et);
		if (et.eType == nilEvent)
			break;
	}
}
	
UInt32 PilotMain( UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
	if (cmd == sysAppLaunchCmdNormalLaunch)
	{
		UInt16 vr;
		UInt32 vi = vfsIteratorStart;
		UInt16 block_count;
		int i;

		print("looking for volumes");
		while (vi != vfsIteratorStop)
		{
			Err e = VFSVolumeEnumerate(&vr, &vi);
			if (e == errNone)
			{
				FileRef fr;

				print("opening file");
				e = VFSFileOpen(vr, "/kernel.img", vfsModeRead, &fr);
				if (e == errNone)
				{
					UInt32 size;

					e = VFSFileSize(fr, &size);
					if (e != errNone)
						goto error;

					print("allocating blocks");
					block_count = (size+BLOCK_SIZE-1) / BLOCK_SIZE;
					for (i=0; i<=block_count; i++)
						blocks[i] = MemPtrNew(BLOCK_SIZE);

					print("sorting blocks");
					SysQSort(blocks, block_count+1, sizeof(*blocks), comparator_cb, 0);

					for (i=0; i<block_count; i++)
					{
						char buffer[64];
						sprintf(buffer, "reading block %d to %p", i, blocks[i]);
						print(buffer);
						VFSFileRead(fr, BLOCK_SIZE, blocks[i], NULL);
					}

					print("copying bootloader");
					memcpy(blocks[block_count], bootloader, sizeof(bootloader));

					print("starting bootloader");
					{
						typedef void (*go_t)(UInt8** blocks, UInt32 block_count);
						go_t cb = (go_t) blocks[block_count];
						cb(blocks, block_count);
					}

				error:
					VFSFileClose(fr);
				}
			}
		}
	}

	return 0;
}
