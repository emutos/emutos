
| ===========================================================================
| ==== sysvars.a - Atari ST system variables definitions ====================
| ===========================================================================

| ===========================================================================
| ==== bss segment ==========================================================
| ===========================================================================

	.bss
| ==== Beginning of RAM (used by OS) ========================================
	.org 0x00000000 	| start of RAM

| ==== Start of System variables ============================================
	.org 0x400		|
etv_timer:     ds.l    1
etv_critic:    ds.l    1
etv_term:      ds.l    1
etv_xtra:      ds.l    5
memvalid:      ds.l    1
	.org 0x424
memctrl:       ds.b    1
	.org 0x426
resvalid:      ds.l    1
resvector:     ds.l    1
phystop:       ds.l    1
_membot:       ds.l    1
_memtop:       ds.l    1
memval2:       ds.l    1
flock:	       ds.w    1
seekrate:      ds.w    1
_timer_ms:     ds.w    1
_fverify:      ds.w    1
_bootdev:      ds.w    1
palmode:       ds.w    1
defshiftmod:   ds.w    1
sshiftmd:      ds.b    1
	.org 0x44e
_v_bas_ad:     ds.l    1
vblsem:        ds.w    1
nvbls:	       ds.w    1
_vblqueue:     ds.l    1
colorptr:      ds.l    1
screenpt:      ds.l    1
_vbclock:      ds.l    1
_frclock:      ds.l    1
hdv_init:      ds.l    1
swv_vec:       ds.l    1
hdvbpb:        ds.l    1
hdv_rw:        ds.l    1
hdv_boot:      ds.l    1
hdv_mediach:   ds.l    1
_cmdload:      ds.w    1
conterm:       ds.b    1
	.org 0x48e
themd:	       ds.l    4
____md:        ds.w    2
savptr:        ds.l    1
_nflops:       ds.w    1
con_state:     ds.l    1
save_row:      ds.w    1
sav_context:   ds.l    1
_bufl:	       ds.l    2
_hz_200:       ds.l    1
the_env:       ds.b    4
	.org 0x4c2
_drvbits:      ds.l    1
_dsk_bufp:     ds.l    1
_autopath:     ds.l    1
_vbl_list:     ds.l    1
_dumpflg:      ds.w    1
_sysbase:      ds.l    1
_shell_p:      ds.l    1
end_os:        ds.l    1
exec_os:       ds.l    1
dump_vec:      ds.l    1
prt_stat:      ds.l    1
prt_vec:       ds.l    1
aux_stat:      ds.l    1
aux_vec:       ds.l    1
memval3:       ds.l    1
bconstat_vec:  ds.l    8
bconin_vec:    ds.l    8
bcostat_vec:   ds.l    8
bconout_vec:   ds.l    8

| ===========================================================================
| ==== End ==================================================================
| ===========================================================================

	.end
