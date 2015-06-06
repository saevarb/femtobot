# Introduction #
This document aims to discuss the requirements, design and implementation of femtobot.

Please note that (_almost_) none of this is absolutely final. Every aspect is most definitely up for discussion, so just make a post on the mailing list if you have an idea or a suggestion. The mailing list email is femtobot-dev@googlegroups.com

Subjects to be discussed:
  * Requirements
  * The general model and design
  * Module support
  * Inter-thread communication **TODO: More here?**
  * Miscellaneous **TODO**
  * main()'s functionality and purpose **TODO**
  * The receiver thread's functionality and purpose **TODO**
  * The sender thread's functionality and purpose **TODO**

# Requirements and Design #
For the initial release(1.0), the bot should support the following:

  * One server
  * Multiple channels with passwords
  * Module support
  * Default module with mandatory functionality. PING handling and perhaps terminal ouput.
Along with the first release, we will release some code that will aid the user in creating a module. E.g., a wrapper function to send data to the sender thread, that would deal with thread-safety so that the user doesn't have to do that directly.
**TODO: What else do we need?**

This is where the _almost_ in the Introduction comes in. These features are the only ones that will be implemented for 1.0, and other features will be left for future versions. This immutability is to make sure we have a well-defined to goal to move towards, and also because this is the first open source project I(And the other founder) have done, and we want to make sure that it actually gets somewhere. Having a well-defined goal will prevent us from getting stuck in our tracks because we keep adding new features to 1.0.

## The general model and design ##
The general program model for the bot would be the main thread, and two auxiliary threads that will deal with sending and receiving data.

On startup, the main thread(e.g. main()) will parse the configuration file, and load the modules, resolve their symbols and attach them to the appropriate event chains.
It will also create the socket(s) and connect to the server(or servers, when we implement multi-server support).

The main thread will, after all essential initialization start the two threads for sending and receiving data, passing an object(struct) with the required data, such as mutexes for coordinating data access, conditional variables to signal various event(such as shutdown), socket file descriptors and possibly more.
It will also parse all received data which will be relayed to it from the reader thread, and start new worker threads for any messages received that have event handlers defined for it.
On top of waiting for the data signal, it will also keep track of a quit condition variable. If this variable is signaled by one of the other threads(If someone decided to implement a !quit command, for example), it will tell all the other threads(read, write and any worker threads) that it is time to quit, wait for their exit and then proceed to clean up memory.
**TODO: Find out what _exactly_ needs to be passed to the threads, without passing _everything everywhere._**

The reader thread will as described before relay read data directly to the main thread, where it will be handled. This is essentially all that the reader thread does. The reader thread does not handle parsing of messages and delegation of work because it is bad design to have a thread manage threads, and it would be painfully annoying to implement.

The writer thread will use a similar concept as the main thread for passing data. It will be waiting on a signal that tells it that data is ready for it, at which point it will read from the shared memory and send that through to the server.
This is all done thread safely, of course. All functions in all modules will be allowed to send data to the server, and a wrapper function that adds to the shared memory in a thread safe manner will be provided by us for custom module development. Please note that it is the module coder's responsibility to ensure that the message sent out to the server is in valid format.

**TODO: Insert flowchart for relationship here**

While the flowchart is missing, I will describe the relationship simply. The model above means that after the initialization, data will only be sent from the reader thread to main, from main to the worker threads in the form of parsed messages and other required data for thread safety(mutexes and so on), and from those worker threads to the writer thread.


The multi-threaded architecture is necessary, since the bot will support modules. The functions exported by these modules will be called on certain events(See section "Module support"), and in the event that these called functions take some time to complete, we cannot have the bot waiting for them to return before it continues execution, as it will result in the bot being unresponsive for the time it takes the function to complete, and it might eventually be disconnected from the server with a "Ping timeout".
**TODO: More here?**

## Module support ##
The way femtobot will support user-made modules and functions is essentially event-based. When a module is added to the bot's configuration files, the user will start by adding a module name, and then, below that, every event that it wants to register, along with the appropriate functions to add to that event-chain. Here's an example of a possible file(This was used for the prototype - the format might change):
```
[module /path/to/module.so]
event=PRIVMSG
functions=privmsg_handler,privmsg_logger
event=NOTICE
functions=notice_handler
```
This format is pretty simple. You start by telling the bot that the following functions all come from the specified module. Then, you specify and event you want the following functions to handle, and then the functions on one line, separated by commas.
In the example config file, the module with the path /path/to/module.so is defined. It has some event-handlers for PRIVMSG, and those event-handlers are privmsg\_handler and privmsg\_logger.
The same applies to the two following lines that define a NOTICE handler.

You can have different modules with different handlers for the same events. You could create another module, add it to the the list and specify PRIVMSG handlers that are in it as well - they will simply be added to the PRIVMSG event chain.

## Inter-thread communication ##
The original intention was to use pipes to make threads communicate, but I realized(Because it was pointed out to me) that using them would be overkill. It would require all the same synchronization as using shared memory, but less friendly. This means that shared memory will be used for communication between threads.