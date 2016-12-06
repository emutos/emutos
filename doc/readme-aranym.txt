EmuTOS - ARAnyM version

This ROM is optimized for ARAnyM:
http://aranym.org/

emutos-aranym.img - Multilanguage

The following optional files are also supplied:
emuicon.rsc - contains additional icons for the desktop
emuicon.def - definition file for the above

The default language is English.
Other languages can be used by setting the NVRAM appropriately.

Alternatively, you can add the -k xx option on the ARAnyM command line
to force a specific language, where xx is:
cz - Czech
de - German
es - Spanish
fi - Finnish
fr - French
gr - Greek
it - Italian
no - Norwegian
ru - Russian (currently unsupported by ARAnyM)
se - Swedish
cd - Swiss German
us - English (US)
uk - English (UK)

The ARAnyM ROM features:
- optimization for 68040 CPU
- no ACSI support
- full NatFeat support (also enabled in the standard 512 KB version)

Note that selecting Norwegian/Swedish currently sets the language to English,
but the keyboard layout to Norwegian/Swedish.

This ROM image has been built using:
make aranym

