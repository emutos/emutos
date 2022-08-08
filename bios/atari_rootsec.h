/*
 * atari_rootsec.h: Atari TOS root sector layout
 *
 * originally derived from Linux definitions (see below)
 *
 * Copyright (C) 2022 The EmuTOS development team
 */
#ifndef ATARI_ROOTSEC_H
#define ATARI_ROOTSEC_H

/*
 * linux/include/linux/atari_rootsec.h
 * definitions for Atari Rootsector layout
 * by Andreas Schwab (schwab@ls5.informatik.uni-dortmund.de)
 *
 * modified for ICD/Supra partitioning scheme restricted to at most 12
 * partitions
 * by Guenther Kelleter (guenther@pool.informatik.rwth-aachen.de)
 */

struct partition_info
{
  UBYTE flg;                    /* bit 0: active; bit 7: bootable */
  char id[3];                   /* "GEM", "BGM", "XGM", or other */
  ULONG st;                     /* start of partition */
  ULONG siz;                    /* length of partition */
};

struct rootsector
{
  char unused[0x156];                   /* room for boot code */
  struct partition_info icdpart[8];     /* info for ICD-partitions 5..12 */
  char unused2[0xc];
  ULONG hd_siz;                         /* size of disk in blocks */
  struct partition_info part[4];
  ULONG bsl_st;                         /* start of bad sector list */
  ULONG bsl_cnt;                        /* length of bad sector list */
  UWORD checksum;                       /* checksum for bootable disks */
};

#endif /* ATARI_ROOTSEC_H */
