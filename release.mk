#
# Makefile - Makefile fragment for building releases
#
# Copyright (c) 2011-2015 The EmuTOS development team.
#
# Authors:
#  VRI      Vincent Rivière
#
# This file is distributed under the GPL, version 2 or at your
# option any later version.  See doc/license.txt for details.
#

#
# This file contains the targets used to build the release archives.
# It is included from the main Makefile.
#

# This subset of the doc directory will be included in all the binary archives
DOCFILES = doc/announce.txt doc/authors.txt doc/bugs.txt doc/changelog.txt \
  doc/license.txt doc/status.txt doc/todo.txt doc/xhdi.txt

# The archives will be placed into this directory
RELEASE_DIR = release-archives

.PHONY: release-clean
NODEP += release-clean
release-clean:
	rm -rf $(RELEASE_DIR)

.PHONY: release-mkdir
NODEP += release-mkdir
release-mkdir:
	mkdir $(RELEASE_DIR)

.PHONY: release-src
NODEP += release-src
RELEASE_SRC = emutos-src-$(VERSION)
release-src:
	mkdir $(RELEASE_DIR)/$(RELEASE_SRC)
	cp -R $(filter-out . .. .git $(RELEASE_DIR), $(shell echo * .*)) $(RELEASE_DIR)/$(RELEASE_SRC)
	find $(RELEASE_DIR)/$(RELEASE_SRC) -type d -exec chmod 755 '{}' ';'
	find $(RELEASE_DIR)/$(RELEASE_SRC) -type f -exec chmod 644 '{}' ';'
	find $(RELEASE_DIR)/$(RELEASE_SRC) -type f -name '*.sh' -exec chmod 755 '{}' ';'
	tar -C $(RELEASE_DIR) --owner=0 --group=0 -zcvf $(RELEASE_DIR)/$(RELEASE_SRC).tar.gz $(RELEASE_SRC)
	rm -r $(RELEASE_DIR)/$(RELEASE_SRC)

.PHONY: release-512k
NODEP += release-512k
RELEASE_512K = emutos-512k-$(VERSION)
release-512k:
	$(MAKE) clean
	$(MAKE) 512
	mkdir $(RELEASE_DIR)/$(RELEASE_512K)
	cp etos512k.img etos512k.sym $(RELEASE_DIR)/$(RELEASE_512K)
	cat doc/readme-512k.txt readme.txt >$(RELEASE_DIR)/$(RELEASE_512K)/readme.txt
	mkdir $(RELEASE_DIR)/$(RELEASE_512K)/doc
	cp $(DOCFILES) $(RELEASE_DIR)/$(RELEASE_512K)/doc
	find $(RELEASE_DIR)/$(RELEASE_512K) -name '*.txt' -exec unix2dos '{}' ';'
	cd $(RELEASE_DIR) && zip -9 -r $(RELEASE_512K).zip $(RELEASE_512K)
	rm -r $(RELEASE_DIR)/$(RELEASE_512K)

.PHONY: release-256k
NODEP += release-256k
RELEASE_256K = emutos-256k-$(VERSION)
release-256k:
	$(MAKE) clean
	$(MAKE) all256
	mkdir $(RELEASE_DIR)/$(RELEASE_256K)
	cp etos256*.img $(RELEASE_DIR)/$(RELEASE_256K)
	cat doc/readme-256k.txt readme.txt >$(RELEASE_DIR)/$(RELEASE_256K)/readme.txt
	mkdir $(RELEASE_DIR)/$(RELEASE_256K)/doc
	cp $(DOCFILES) $(RELEASE_DIR)/$(RELEASE_256K)/doc
	find $(RELEASE_DIR)/$(RELEASE_256K) -name '*.txt' -exec unix2dos '{}' ';'
	cd $(RELEASE_DIR) && zip -9 -r $(RELEASE_256K).zip $(RELEASE_256K)
	rm -r $(RELEASE_DIR)/$(RELEASE_256K)

.PHONY: release-192k
NODEP += release-192k
RELEASE_192K = emutos-192k-$(VERSION)
release-192k:
	$(MAKE) clean
	$(MAKE) all192
	mkdir $(RELEASE_DIR)/$(RELEASE_192K)
	cp etos192*.img $(RELEASE_DIR)/$(RELEASE_192K)
	cat doc/readme-192k.txt readme.txt >$(RELEASE_DIR)/$(RELEASE_192K)/readme.txt
	mkdir $(RELEASE_DIR)/$(RELEASE_192K)/doc
	find $(RELEASE_DIR)/$(RELEASE_192K) -name '*.txt' -exec unix2dos '{}' ';'
	cp $(DOCFILES) $(RELEASE_DIR)/$(RELEASE_192K)/doc
	find $(RELEASE_DIR)/$(RELEASE_192K) -name '*.txt' -exec unix2dos '{}' ';'
	cd $(RELEASE_DIR) && zip -9 -r $(RELEASE_192K).zip $(RELEASE_192K)
	rm -r $(RELEASE_DIR)/$(RELEASE_192K)

.PHONY: release-cartridge
NODEP += release-cartridge
RELEASE_CARTRIDGE = emutos-cartridge-$(VERSION)
release-cartridge:
	$(MAKE) clean
	$(MAKE) cart
	mkdir $(RELEASE_DIR)/$(RELEASE_CARTRIDGE)
	cp $(ROM_CARTRIDGE) $(RELEASE_DIR)/$(RELEASE_CARTRIDGE)
	cp emutos.stc $(RELEASE_DIR)/$(RELEASE_CARTRIDGE)
	cat doc/readme-cartridge.txt readme.txt >$(RELEASE_DIR)/$(RELEASE_CARTRIDGE)/readme.txt
	mkdir $(RELEASE_DIR)/$(RELEASE_CARTRIDGE)/doc
	cp $(DOCFILES) $(RELEASE_DIR)/$(RELEASE_CARTRIDGE)/doc
	find $(RELEASE_DIR)/$(RELEASE_CARTRIDGE) -name '*.txt' -exec unix2dos '{}' ';'
	cd $(RELEASE_DIR) && zip -9 -r $(RELEASE_CARTRIDGE).zip $(RELEASE_CARTRIDGE)
	rm -r $(RELEASE_DIR)/$(RELEASE_CARTRIDGE)

.PHONY: release-aranym
NODEP += release-aranym
RELEASE_ARANYM = emutos-aranym-$(VERSION)
release-aranym:
	$(MAKE) clean
	$(MAKE) aranym
	mkdir $(RELEASE_DIR)/$(RELEASE_ARANYM)
	cp $(ROM_ARANYM) $(RELEASE_DIR)/$(RELEASE_ARANYM)
	cat doc/readme-aranym.txt readme.txt >$(RELEASE_DIR)/$(RELEASE_ARANYM)/readme.txt
	mkdir $(RELEASE_DIR)/$(RELEASE_ARANYM)/doc
	cp $(DOCFILES) $(RELEASE_DIR)/$(RELEASE_ARANYM)/doc
	find $(RELEASE_DIR)/$(RELEASE_ARANYM) -name '*.txt' -exec unix2dos '{}' ';'
	cd $(RELEASE_DIR) && zip -9 -r $(RELEASE_ARANYM).zip $(RELEASE_ARANYM)
	rm -r $(RELEASE_DIR)/$(RELEASE_ARANYM)

.PHONY: release-firebee
NODEP += release-firebee
RELEASE_FIREBEE = emutos-firebee-$(VERSION)
release-firebee:
	$(MAKE) clean
	$(MAKE) firebee
	mkdir $(RELEASE_DIR)/$(RELEASE_FIREBEE)
	cp $(SREC_FIREBEE) $(RELEASE_DIR)/$(RELEASE_FIREBEE)
	cat doc/readme-firebee.txt readme.txt >$(RELEASE_DIR)/$(RELEASE_FIREBEE)/readme.txt
	mkdir $(RELEASE_DIR)/$(RELEASE_FIREBEE)/doc
	cp $(DOCFILES) $(RELEASE_DIR)/$(RELEASE_FIREBEE)/doc
	find $(RELEASE_DIR)/$(RELEASE_FIREBEE) -name '*.txt' -exec unix2dos '{}' ';'
	cd $(RELEASE_DIR) && zip -9 -r $(RELEASE_FIREBEE).zip $(RELEASE_FIREBEE)
	rm -r $(RELEASE_DIR)/$(RELEASE_FIREBEE)

.PHONY: release-amiga
NODEP += release-amiga
RELEASE_AMIGA = emutos-amiga-$(VERSION)
release-amiga:
	$(MAKE) clean
	$(MAKE) amigakd
	mkdir $(RELEASE_DIR)/$(RELEASE_AMIGA)
	cp $(ROM_AMIGA) $(RELEASE_DIR)/$(RELEASE_AMIGA)
	cp $(AMIGA_KICKDISK) $(RELEASE_DIR)/$(RELEASE_AMIGA)
	cat doc/readme-amiga.txt readme.txt >$(RELEASE_DIR)/$(RELEASE_AMIGA)/readme.txt
	mkdir $(RELEASE_DIR)/$(RELEASE_AMIGA)/doc
	cp $(DOCFILES) $(RELEASE_DIR)/$(RELEASE_AMIGA)/doc
	find $(RELEASE_DIR)/$(RELEASE_AMIGA) -name '*.txt' -exec unix2dos '{}' ';'
	cd $(RELEASE_DIR) && zip -9 -r $(RELEASE_AMIGA).zip $(RELEASE_AMIGA)
	rm -r $(RELEASE_DIR)/$(RELEASE_AMIGA)

.PHONY: release-m548x-dbug
NODEP += release-m548x-dbug
RELEASE_M548X_DBUG = emutos-m548x-dbug-$(VERSION)
release-m548x-dbug:
	$(MAKE) clean
	$(MAKE) m548x-dbug
	mkdir $(RELEASE_DIR)/$(RELEASE_M548X_DBUG)
	cp $(SREC_M548X_DBUG) $(RELEASE_DIR)/$(RELEASE_M548X_DBUG)
	cat doc/readme-m548x-dbug.txt readme.txt >$(RELEASE_DIR)/$(RELEASE_M548X_DBUG)/readme.txt
	mkdir $(RELEASE_DIR)/$(RELEASE_M548X_DBUG)/doc
	cp $(DOCFILES) $(RELEASE_DIR)/$(RELEASE_M548X_DBUG)/doc
	find $(RELEASE_DIR)/$(RELEASE_M548X_DBUG) -name '*.txt' -exec unix2dos '{}' ';'
	cd $(RELEASE_DIR) && zip -9 -r $(RELEASE_M548X_DBUG).zip $(RELEASE_M548X_DBUG)
	rm -r $(RELEASE_DIR)/$(RELEASE_M548X_DBUG)

.PHONY: release-m548x-bas
NODEP += release-m548x-bas
RELEASE_M548X_BAS = emutos-m548x-bas-$(VERSION)
release-m548x-bas:
	$(MAKE) clean
	$(MAKE) m548x-bas
	mkdir $(RELEASE_DIR)/$(RELEASE_M548X_BAS)
	cp $(SREC_M548X_BAS) $(RELEASE_DIR)/$(RELEASE_M548X_BAS)
	cat doc/readme-m548x-bas.txt readme.txt >$(RELEASE_DIR)/$(RELEASE_M548X_BAS)/readme.txt
	mkdir $(RELEASE_DIR)/$(RELEASE_M548X_BAS)/doc
	cp $(DOCFILES) $(RELEASE_DIR)/$(RELEASE_M548X_BAS)/doc
	find $(RELEASE_DIR)/$(RELEASE_M548X_BAS) -name '*.txt' -exec unix2dos '{}' ';'
	cd $(RELEASE_DIR) && zip -9 -r $(RELEASE_M548X_BAS).zip $(RELEASE_M548X_BAS)
	rm -r $(RELEASE_DIR)/$(RELEASE_M548X_BAS)

.PHONY: release-prg
NODEP += release-prg
RELEASE_PRG = emutos-prg-$(VERSION)
release-prg:
	$(MAKE) clean
	$(MAKE) allprg
	mkdir $(RELEASE_DIR)/$(RELEASE_PRG)
	cp emutos*.prg $(RELEASE_DIR)/$(RELEASE_PRG)
	cat doc/readme-prg.txt readme.txt >$(RELEASE_DIR)/$(RELEASE_PRG)/readme.txt
	mkdir $(RELEASE_DIR)/$(RELEASE_PRG)/doc
	cp $(DOCFILES) $(RELEASE_DIR)/$(RELEASE_PRG)/doc
	find $(RELEASE_DIR)/$(RELEASE_PRG) -name '*.txt' -exec unix2dos '{}' ';'
	cd $(RELEASE_DIR) && zip -9 -r $(RELEASE_PRG).zip $(RELEASE_PRG)
	rm -r $(RELEASE_DIR)/$(RELEASE_PRG)

.PHONY: release-floppy
NODEP += release-floppy
RELEASE_FLOPPY = emutos-floppy-$(VERSION)
release-floppy:
	$(MAKE) clean
	$(MAKE) flop UNIQUE=us
	mkdir $(RELEASE_DIR)/$(RELEASE_FLOPPY)
	cp emutos.st $(RELEASE_DIR)/$(RELEASE_FLOPPY)
	cat doc/readme-floppy.txt readme.txt >$(RELEASE_DIR)/$(RELEASE_FLOPPY)/readme.txt
	mkdir $(RELEASE_DIR)/$(RELEASE_FLOPPY)/doc
	cp $(DOCFILES) $(RELEASE_DIR)/$(RELEASE_FLOPPY)/doc
	find $(RELEASE_DIR)/$(RELEASE_FLOPPY) -name '*.txt' -exec unix2dos '{}' ';'
	cd $(RELEASE_DIR) && zip -9 -r $(RELEASE_FLOPPY).zip $(RELEASE_FLOPPY)
	rm -r $(RELEASE_DIR)/$(RELEASE_FLOPPY)

# Main goal to build a full release distribution
.PHONY: release
NODEP += release
release: distclean release-clean release-mkdir \
  release-src release-512k release-256k release-192k release-cartridge \
  release-aranym release-firebee release-amiga release-m548x-dbug \
  release-m548x-bas release-ram release-floppy
	$(MAKE) clean
	@echo '# Packages successfully generated inside $(RELEASE_DIR)'
