et - the extraterrestrial botnet
===

### What is it?
`et` is a dumb client to an IRC command-and-control server.

When activated, it "phones home" and awaits instructions from members of an IRC channel.

It can be instructed to retrieve system information and execute commands, among other things.

### Compilation

On *nix systems (Linux, BSD, OS X):
```bash
$ git clone https://github.com/woodruffw/et && cd et
$ make nix
```
Do not actually run the resulting `et` binary, as it *WILL* connect you to the default server and open your computer to attack.

The Windows version is incomplete.

### Usage
Controlling the `et` bots from the IRC server is very simple.

When each bot connects, it sends out a channel-wide message of the format:
```
et#######: et####### phoning home
```

From that point on, the bot is ready to receive commands. All commands are executed by `/bin/sh -c` expect for quit and info, which are defined to kill the bot and report `uname(3)` output respectively.

Some examples, all from the IRC prompt:
```
et#######: et####### phoning home
/msg et####### info
/msg et####### pwd
/msg et####### quit
```

Currently, output from system commands is limited to approximately 512 characters. 

### Proof of Concept
The original PoC Python script can be found under `src/poc/et.py`. Like the actual version, it does work and *WILL* expose your machine.

### Theoretical Limits
Because the IRC protocol limits nicks to 9 characters, only so many `et`s can operate within a single channel.

By default, the first two characters of an `et`'s nick will always be 'et,' meaning that at most 10 million `et`s can exist within a channel.
Of course, because the trailing numbers are pseudorandom and not sequential, the actual point of collision is likely to be much lower than 10 million.
On the other hand, `et` can be configured to "phone home" to other servers and channels, making the limit less relevant in practice.

### Status
`et` is currently a work in progress and is not fully complete.

Many of its features are currently missing.