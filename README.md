et - the extraterrestrial botnet
===

### What is it?
`et` is a dumb client to an IRC command-and-control server.

When activated, it "phones home" and awaits instructions from members of an IRC channel.

It can be instructed to retrieve system information and execute commands, among other things.

### Theoretical Limits
Because the IRC protocol limits nicks to 9 characters, only so many `et`s can operate within a single channel.

By default, the first two characters of an `et`'s nick will always be 'et,' meaning that at most 10 million `et`s can exist within a channel.
Of course, because the trailing numbers are pseudorandom and not sequential, the actual point of collision is likely to be much lower than 10 million.
On the other hand, `et` can be configured to "phone home" to other servers and channels, making the limit less relevant in practice.

### Status
`et` is currently a work in progress and is not fully complete.

Many of its features are currently missing.