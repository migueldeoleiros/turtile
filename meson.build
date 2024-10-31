project(
	'turtile', 
	['c'],
	version: '0.0.1',
	license: 'GPL-3.0',
)

add_global_arguments('-DWLR_USE_UNSTABLE=1', language : 'c')

#
# Wayland - Protocols
#
wlproto = dependency('wayland-protocols')
wlproto_dir = wlproto.get_variable(pkgconfig: 'pkgdatadir')
wlscan = dependency('wayland-scanner', required: false, native: true)
if wlscan.found()
  wl_scanner = find_program(wlscan.get_variable(pkgconfig: 'wayland_scanner'), native: true)
else
  wl_scanner = find_program('wayland-scanner', native: true)
endif

xdg_shell_xml = wlproto_dir / 'stable/xdg-shell/xdg-shell.xml'
xdg_shell_h = custom_target(
  'xdg-shell-protocol.h',
  output: 'xdg-shell-protocol.h',
  input: xdg_shell_xml,
  command: [wl_scanner, 'server-header', '@INPUT@', '@OUTPUT@'],
)

deps = [
	dependency('wlroots-0.18'),
	dependency('wayland-server'),
	dependency('xkbcommon'),
	dependency('libconfig'),
    dependency('json-c'),
    dependency('uuid')
]

executable(
	'turtile',
  [
    'src/commands.c',
    'src/config.c',
    'src/cursor.c',
    'src/keyboard.c',
    'src/main.c',
    'src/output.c',
    'src/popup.c',
    'src/server.c',
    'src/socket_server.c',
    'src/toplevel.c',
    'src/workspace.c',
    xdg_shell_h
  ],
	dependencies : deps
)

executable(
	'ttcli',
  [
    'src/ttcli.c',
  ],
	dependencies : deps
)

