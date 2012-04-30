# Getting Started

To build the backup system, you presently need a recent Linux distribution, and
the following libraries:

 * [CMake](http://www.cmake.org/) 2.8 or later
 * [Boost](http://www.boost.org/)
 * [ZeroC ICE](http://www.zeroc.com/) 3.4
 * [Google Logging Libraries (glog)](http://code.google.com/p/google-glog/)
 * [Google Command-Line Flags Libraries] (http://code.google.com/p/gflags/?redir=1)
 * Essential build tools (gcc, make, git, etc.)

I highly recommend installing the Google command-line flags library before the
logging library, and then compiling the logging library from source.  The two
work together, and most distributions don't compile them together, so you end up
having to do things like:

    $ GLOG_logtostderr=1 GLOG_vmodule=my_module=3 ./binary

rather than the much nicer:

    $ ./binary --logtostderr --vmodule=my_module=3

# Building

In a nutshell:

    $ cd /path/to/backup.git
    $ cmake . && make

# Using

Presently, there's two binaries produced during the build:

 * A command-line tool, `cli_client`, which can be used to test the backup 
   system, and
 * A BTRFS-based server daemon, `btrfs_backend`, which implements the 
   server-side half of the BTRFS backend.

The tools are located in the source tree after build:

 * `client/ui/cli/cli_client`
 * `backend/btrfs/btrfs_backend`

Both support `--help`, but the gist of running them is like this.  First, start
the backend:

    $ ./backend/btrfs/btrfs_backend --backend_path=/tmp/backup --logtostderr

The `--backend_path` is the location where actual backup data is stored; it can
be anywhere.  If `--logtostderr` is not specified, logs are stored in /tmp.

Now, the client can be used:

    $ ./client/ui/cli/cli_client --backend=btrfs create_backup_set "My Backup Set"
    $ ./client/ui/cli/cli_client --backend=btrfs list_backup_sets
    ...

# Development

You can find an up-to-date design document
[here](https://docs.google.com/document/d/1lEYwbH4FO_eQTLCwedT7ZtwtqF2xrC6fLtDrgptP9G8/edit).

The source tree is split into several parts:

 * `backend/`: Storage backend implementations
 * `base/`: Headers that are generally useful.
 * `client/`: Implementation of client front-end interfaces, such as the GUI 
    and CLI interfaces.
 * `docs/`: Various documentation files.
 * `meta/`: CMake scripts needed for the build

The project is using the
[Google C++ Style Guide](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml)
as the basis for coding style.

To contribute to the project, create a fork of the repository on GitHub and do
your developement.  Then, submit a pull request at the project's GitHub site
[here](https://github.com/darkstar62/backup) to participate in a code review.
