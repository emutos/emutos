This release has been built using Vincent Rivière's GCC 4.6.4 cross-compiler.

The source package and other binary packages are available at:
https://sourceforge.net/projects/emutos/files/emutos/1.4/

An online manual is available at the following URL:
https://emutos.github.io/manual/

The extras directory (if provided) contains:
(1) one or more alternate desktop icon sets, which you can use to replace
    the builtin ones.  You can use a standard resource editor to see what
    the replacement icons look like.
    To use a replacement set, move or rename the existing emuicon.rsc &
    emuicon.def files in the root directory, then copy the files containing
    the desired icons to the root, and rename them to emuicon.rsc/emuicon.def.
(2) a sample mouse cursor set in a resource (emucurs.rsc/emucurs.def).  This
    set is the same as the builtin ones, but you can use it as a basis to
    create your own mouse cursors.
    To use a replacement set, copy the files containing the desired mouse
    cursors to the root, and rename them to emucurs.rsc/emucurs.def.
For further information on the above, see doc/emudesk.txt.

If you want to read more about EmuTOS, please take a look at these files:

doc/announce.txt      - Introduction and general description, including
                        a summary of changes since the previous version
doc/authors.txt       - A list of the authors of EmuTOS
doc/bugs.txt          - Currently known bugs
doc/changelog.txt     - A summarised list of changes after release 0.9.4
doc/emudesk.txt       - A brief guide to the newer features of the desktop
doc/incompatible.txt  - Programs incompatible with EmuTOS due to program bugs
doc/license.txt       - The FSF General Public License for EmuTOS
doc/status.txt        - What is implemented and running (or not yet)
doc/todo.txt          - What should be done in future versions
doc/tools.txt         - Tools to customize EmuTOS ROM images
doc/xhdi.txt          - Current XHDI implementation status

Additional information for developers (just in the source archive):

doc/install.txt       - How to build EmuTOS from sources
doc/coding.txt        - EmuTOS coding standards (never used :-) )
doc/country.txt       - An overview of i18n issues in EmuTOS
doc/dual_kbd.txt      - An explanation of the dual keyboard layout feature
doc/fat16.txt         - Notes on the FAT16 filesystem in EmuTOS
doc/m54xx-cards.txt   - Using CF cards on ColdFire V4e Evaluation Boards
doc/memdetect.txt     - Memory bank detection during EmuTOS startup
doc/nls.txt           - How to add a native language or use one
doc/old_changelog.txt - A summarised list of changes up to & including
                        release 0.9.4
doc/osmemory.txt      - All about OS internal memory in EmuTOS
doc/reschange.txt     - How resolution change works in the desktop
doc/resource.txt      - Modifying resources in EmuTOS
doc/startup.txt       - Some notes on the EmuTOS startup sequence
doc/tools.txt         - User tools to customise EmuTOS ROM images
doc/tos14fix.txt      - Lists bugs fixed by TOS 1.04 & their status in EmuTOS
doc/version.txt       - Determining the version of EmuTOS at run-time

The following documents are principally of historical interest only:

doc/old_code.txt      - A museum of bugs due to old C language
doc/vdibind.txt       - Old information on VDI bindings

-- 
The EmuTOS development team
https://emutos.sourceforge.io/
