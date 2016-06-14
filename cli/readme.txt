EmuCON2 - EmuTOS console, standalone version

EmuCON2 is a basic but useful command-line interpreter, written from
scratch by Roger Burrows in 2013 to replace the original EmuTOS CLI.

It requires approximately 30kB, works with plain TOS, and supports
history, line editing and TAB completion for file names.  Command
names are a combination of DOS and Un*x:
    cat/type
    cd
    chmod
    cls/clear
    cp/copy
    echo
    exit
    help
    ls/dir
    mkdir/md
    mode
    more
    mv/move
    path
    pwd
    ren
    rm/del
    rmdir/rd
    show
    version
    wrap

The 'help' command shows the (above) list of commands, or individual
help for a specific command, like this:
------------------------------------------
C:>help show
show [<drive>]
    Show info for <drive> or current drive
------------------------------------------

Like the rest of EmuTOS, EmuCON2 is Open Source, so any bugs in it can
be (eventually) fixed.  Unlike the rest of EmuTOS, it is available only
in English.
