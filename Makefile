WAYLAND_PROTOCOLS=$(shell pkg-config --variable=pkgdatadir wayland-protocols)
WAYLAND_SCANNER=$(shell pkg-config --variable=wayland_scanner wayland-scanner)
LIBS=\
	 $(shell pkg-config --cflags --libs wlroots-0.18) \
	 $(shell pkg-config --cflags --libs wayland-server) \
	 $(shell pkg-config --cflags --libs xkbcommon) \
	 $(shell pkg-config --cflags --libs libconfig)

CFLAGS=-g -Werror -I. -DWLR_USE_UNSTABLE 

# wayland-scanner is a tool which generates C headers and rigging for Wayland
# protocols, which are specified in XML. wlroots requires you to rig these up
# to your build system yourself and provide them in the include path.
xdg-shell-protocol.h:
	$(WAYLAND_SCANNER) server-header \
		$(WAYLAND_PROTOCOLS)/stable/xdg-shell/xdg-shell.xml $@

turtile: src/main.c src/cursor.c src/keyboard.c src/output.c src/server.c src/toplevel.c src/popup.c xdg-shell-protocol.h
	$(CC) $(CFLAGS) \
		-g -Werror -I. \
		-DWLR_USE_UNSTABLE \
		-o $@ $< src/cursor.c src/keyboard.c src/output.c src/server.c src/toplevel.c src/popup.c \
		$(LIBS)

clean:
	rm -f turtile xdg-shell-protocol.h xdg-shell-protocol.c *.o

.DEFAULT_GOAL=turtile
.PHONY: clean
