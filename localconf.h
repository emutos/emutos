#define NUM_VDI_HANDLES 32
#define CONF_WITH_TTRAM 0
#define CONF_WITH_MONSTER 0
#define CONF_WITH_MAGNUM 0
#define CONF_WITH_VME 0
#define CONF_WITH_NOVA 0
#define INITINFO_DURATION 2
#define CONF_WITH_COLOUR_ICONS 0
#define CONF_WITH_EXTENDED_MOUSE 0
#define CONF_WITH_LOADABLE_CURSORS 0
#define CONF_WITH_CACHE_CONTROL 0
#define CONF_WITH_EASTER_EGG 0
#define CONF_WITH_PRINTER_ICON 0
#define CONF_WITH_VDI_VERTLINE 1


/* If blitter is available, always use it and remove software drawing routines. */
/* #define MPS_BLITTER_ALWAYS_ON 1 */

/* Use lookup table for line offsets rather than multiply in vdi_misc */
/* #define MPS_LINES_LUT 1 */
#define MPS_LINES_LUT 1

/* These are mutually exclusive */
#define MPS_STF 1
#define MPS_STE 0

#if MPS_STF
  /* STf have no blitter so remove related code.
   * Mega ST are a different beast which is not supported here (rare enough). */
  #define CONF_WITH_BLITTER 0
  #define MPS_BLITTER_ALWAYS_ON 0
  #define ASM_BLIT_IS_AVAILABLE 1
#endif

#if MPS_STE
  /* As blitter is available, always use it and don't include software drawing/blitting routines */
  #define CONF_WITH_BLITTER 1
  #define ASM_BLIT_IS_AVAILABLE 0
  #define MPS_BLITTER_ALWAYS_ON 1
#endif