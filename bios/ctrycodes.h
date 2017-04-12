/*
 * ctrycodes.h - a mere list of country codes
 *
 * Copyright (C) 2003-2017 The EmuTOS development team
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * The country codes were defined by Atari. They do not need to be contiguous.
 * They are used as the country identifier in the ROM header.
 * They are also used in NVRAM to select the UI language and keyboard layout.
 */

/* these are documented in the Compendium */
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
 * https://github.com/freemint/freemint/blob/master/sys/keyboard.c
 * https://github.com/freemint/freemint/blob/master/xaaes/src.km/init.c
 */
#define COUNTRY_RU 19   /* Russia */
#define COUNTRY_GR 31   /* Greece */

/*
 * Special value of os_conf to indicate that the display and keyboard languages
 * will be read from the NVRAM. If the NVRAM is invalid, the default settings
 * will be inferred from the COUNTRY Makefile variable.
 */
#define OS_CONF_MULTILANG 0xff
