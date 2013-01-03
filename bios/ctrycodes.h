/*
 * ctrycodes.h - a mere list of country codes
 *
 * Copyright (c) 2003 EmuTOS development team.
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * The country codes were defined by Atari. They do not need to be contiguous.
 * They are used as country identifier in the ROM header.
 * They are also used in NVRAM to select the UI language and keyboard layout.
 */

/* these are documented in the compendium */
#define COUNTRY_US  0   /* USA */
#define COUNTRY_DE  1   /* Germany */
#define COUNTRY_FR  2   /* France */
#define COUNTRY_UK  3   /* United Kingdom */
#define COUNTRY_ES  4   /* Spain */
#define COUNTRY_IT  5   /* Italy */
#define COUNTRY_SE  6   /* Sweden */
#define COUNTRY_SF  7   /* Switzerland (French) */
#define COUNTRY_SG  8   /* Switzerland (German), NOT Singapore! */
#define COUNTRY_TR  9   /* Turkey */
#define COUNTRY_FI 10   /* Finland */
#define COUNTRY_NO 11   /* Norway */
#define COUNTRY_DK 12   /* Denmark */
#define COUNTRY_SA 13   /* Saudi Arabia */
#define COUNTRY_NL 14   /* Holland */
#define COUNTRY_CZ 15   /* Czech Republic */
#define COUNTRY_HU 16   /* Hungary */

/*
 * The following country codes were not defined by Atari.
 * Before defining new ones, be sure to register them in tos.hyp:
 * http://toshyp.atari.org/en/003007.html#Cookie_2C_20_AKP
 * Note that those codes are also used in FreeMiNT/XaAES and must match:
 * http://sparemint.atariforge.net/cgi-bin/cvsweb/freemint/sys/keyboard.c?rev=1.112&content-type=text/x-cvsweb-markup
 * http://sparemint.atariforge.net/cgi-bin/cvsweb/freemint/xaaes/src.km/init.c?rev=1.123&content-type=text/x-cvsweb-markup
 */
#define COUNTRY_GR 31   /* Greece */
#define COUNTRY_RU 32   /* Russia */
