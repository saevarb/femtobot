# Introduction #
Femtobot is an IRC bot in the C programming language. The intention is to create a bot that will support modules in the form of shared objects on Linux(And DLLs on windows, when we get that far), making it extremely customizable. The bot will be event-based, in that the modules' exported symbols will be called when a particular message is received from the server. By message, I mean a real message defined by the IRC protocol, not a user-defined message commonly in the format "!command", although this will be supported as well.

At the time of writing, this project is still very new, and the final design is still being discussed.

The aim of this project is, above everything else, to be a learning experience for the coders, as we, the founders, are somewhat experienced hobbyist programmers looking for some experience before we head out in the real world.
As such, we would not mind new programmers at all, as long as they have some experience with multi-threaded programming, network programming and if possible, dynamically linked libraries on either Windows or Linux/Unix(Windows knowledge can be applied to Linux/Unix and the other way around).

I assume it goes without saying that anyone with more experience than we have would be appreciated as well. ;)

Mailing list: femtobot-dev@googlegroups.com