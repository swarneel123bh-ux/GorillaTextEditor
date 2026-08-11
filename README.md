# GorillaTextEditor

A modal terminal text editor written in C using ncurses, following Vim's editing
model.

## Design

The editor is a state machine over four modes, dispatched from a single loop in
`main.c`. Each mode handler returns a signal (`SIGNAL_SWITCH_TO_INPUTMODE`,
`SIGNAL_EXIT_PROGRAM`, and so on) rather than changing global state directly, so
mode transitions are all resolved in one place.

| Mode | Entered by | Purpose |
|------|-----------|---------|
| Normal | `Esc` | Navigation and operators |
| Input | `i` `I` `a` `A` `o` `O` | Text entry |
| Visual | `v` `V` | Selection, copy, cut |
| Command | `:` | File operations |

The screen is split into two ncurses windows: an editing pane and a command/
message pane, each drawn inside a box. Text lives in a `lineArr` of `line`
structs, where each `line` carries its own allocated size and occupied length so
lines grow independently.

## Features

- **Navigation** — `hjkl` and arrow keys, `Home`/`End`, in both normal and input modes.
- **Insertion** — `i`/`I` at cursor or line start, `a`/`A` after cursor or at line end, `o`/`O` to open a line below or above.
- **Visual mode** — character and line selection with live highlighting, `c` to copy, `x` to cut.
- **Clipboard** — multi-line copy, cut and paste with `p`/`P`, backed by a dedicated clipboard buffer.
- **Commands** — `:w` write, `:q` quit, `:q!` force quit, `:wq` write and quit. `:q` refuses to exit when there are unsaved changes; `:q!` overrides.
- **File I/O** — loads a file given as `argv[1]`, or starts on an untitled buffer.

## Building

Requires `gcc`, `make`, and ncurses. The Makefile detects Windows, Linux and
macOS, and builds separate optimised and debug binaries with header dependency
tracking.

```sh
make release      # bin/main,    -O2 -DNDEBUG
make debug        # bin/maindbg, -g -O0
make run          # build and run the release binary
make run_debug    # build and run the debug binary
make clean
```

## Buffer design and its limits

Text is held in a flat array of `line*`, with all `MAXBUFLEN` (1000) line
structs allocated up front at startup. This keeps line lookup O(1) and
insertion a single `memmove` over a pointer array, which is cheap — but it
means the editor pays for 1000 lines of allocation whether the file needs
them or not, and cannot open a file longer than that at all.

The intended fix is a chunked buffer: a linked list of fixed-size blocks of
line structs, with chunks loaded on demand rather than all at once. This is
essentially a rope with lazily materialised leaves. An index of chunk
boundaries stays resident even when chunk contents are not, so line jumps and
search still work across unloaded regions.

Other current limitations:

- No undo/redo and no syntax highlighting yet.
- The `min`/`max` macros are unparenthesised, so they will misbehave inside
  larger expressions.

## Planned

- Chunked buffer with on-demand loading
- Undo/redo
- Syntax highlighting
- Automatic indentation
