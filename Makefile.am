SUBDIRS =  m4 include po src intl Documentation Packages Robots Arenas $(subdirs)

rtbextradir = $(RTB_DIR)
rtbextra_DATA = RealTimeBattle.xpm tournament.txt rtblogo1024.jpg  rtblogo1600.jpg  rtb_logo.jpg
EXTRA_DIST = config/config.rpath config/mkinstalldirs  config/config.rpath   $(rtbextra_DATA)

debian freebsd rpm:
	cd Packages/$@ && $(MAKE) $@

packages:
	cd Packages && $(MAKE) packages

ACLOCAL_AMFLAGS = -I m4 -I config

