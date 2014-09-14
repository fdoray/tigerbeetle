tigerbeetle
===========

![A tiger beetle](http://0x3b.org/ss/parosteosis555.png)

[![Build Status](https://travis-ci.org/fdoray/tigerbeetle.svg?branch=master)](https://travis-ci.org/fdoray/tigerbeetle)

***tigerbeetle*** is an ongoing project aiming at the creation of a
fast, flexible trace analysis core. The project name has absolutely no
meaning and is subject to change.

This document describes the foundations and ideas behind tigerbeetle:

  * **easy-to-use**
  * **extensible**
  * **modern**

But first, a few concepts.


concepts
--------

You will need to understand a few concepts before diving into tigerbeetle.


### tracing

According to Wikipedia:

> In software engineering, _tracing_ is a specialized use of logging to
> record information about a program's execution. This information is
> typically used by programmers for debugging purposes, and additionally,
> depending on the type and detail of information contained in a trace log,
> by experienced system administrators or technical support personnel and
> software monitoring tools to diagnose common problems with software.

You may view a trace as a log file, each _tracepoint_ producing an _event_.
The biggest difference between tracing and logging is that tracing is normally
way faster and isn't limited to recording lines of plain text.

The software behind tracing is called, believe it or not, a _tracer_. Its
purpose is basically to collect generated events and store them somewhere
(usually in a file) as fast as possible.

Since a tracer is so fast, it's possible to record events that happen very
frequently, e.g. all operating system calls, without losing any. This, of
course, has the effect of producing huge trace files with a loooot of
information. Listing the recorded events provides a certain view of the trace
(and is the easiest to implement), but is often useless because all this
information could be reduced, depending on the application, to a few
relevant results with much less details.

Here's an example. [Wireshark](http://www.wireshark.org/) can be seen as
a network tracer. Wireshark events are individual network packets. The
default view, when capturing, presents a list of events. While this allows
you to solve problems by inspecting individual packets, Wireshark also
provides other interesting analysis tools: statistics, filtering, TCP stream
following, etc.

In order to get a custom analysis ouf of the events of a trace, you need some
sort of state machine. tigerbeetle is all about easily implementing your own
state machine and using it with any trace.


### stateful analysis

If you read the last paragraph, we mentioned _implementing your own state
machine_. What does it mean?

The concept of a _state_ is super simple, so much that you already know what
it is. But here's a [formal definition](http://www.thefreedictionary.com/state)
which applies to our context:

> The condition of a person or thing, as with respect to circumstances
> or attributes.

_Circumstances or attributes_, in our case, are events with the data they
carry (timestamp, payload, context, etc.). So here's what the tigerbeetle
asks you, the developer/analyzer, to do: modify a state object as you receive
recorded events in the same order they happened. Can you do that?

Here's an example. You are the author of a famous audio player and you notice
glitches in the sound when using the MP3 codec and receiving the compressed
audio data over the network.

You place a few tracepoints at critical locations in the code:

  * add data to buffer of received data
  * get data from buffer of received data
  * start decoding an MP3 frame

Now, you start the audio player and listen to an MP3 song downloaded as it
is played; the tracer records all the generated events and stores them in a
trace file. You hear glitches as usual.

You could look at the list of individual events, but what you really want to
know here is: does the buffer of received data become full at any time, and
if so, which MP3 frame is the player decoding when this happens?

Here's some Python code demonstrating how to build your state object:

```python
def on_init(state):
    state['net_buffer'] = 0
    state['current_frame_bitrate'] = 0
    state['alert'] = 'no'

def on_event(event, state):
    if event.type == 'net_add_data':
        state['net_buffer'] += event['size']
    if event.type == 'net_get_data':
        state['net_buffer'] -= event['size']
    if event.type == 'decode_frame':
        state['current_frame_bitrate'] = event['bitrate']

    if state['net_buffer'] >= 16 * 1024:
        state['alert'] = 'yes'
    else:
        state['alert'] = 'no'
```

Now, let's pretend you have an appropriate tool to inspect the system state
in time and display a red bar when the `alert` attribute is `yes`. You would
look at all those red moments and check the bitrate of the frame currently
being decoded. And you would find what you suspected: 320 kbps frames take
too much time to be decoded, leading to the network input buffer being full
and some received frames being dropped, hence the strange sound _sometimes_.
And you would probably optimize the decoding algorithm. Or stop testing with
a Pentium II-based PC.

What we just wrote in Python above is called a _state provider_. It's a little
piece of user-written code allowing the custom analysis of any application.


presenting tigerbeetle
----------------------

Now that you are familiar with a few important concepts, here's an informal
presentation of the tigerbeetle project.

Our goal is to first focus on the [LTTng](http://lttng.org/) tracer.
[There](https://sourceware.org/systemtap/)
[are](http://www.sysdig.org/)
[others](http://en.wikipedia.org/wiki/DTrace) out
there, but LTTng is a powerful, modern, stable and actively maintained kernel
and userspace tracer for Linux. Some analysis tools exist for inspecting
LTTng traces, but they are either:

  * too simple (list of events)
  * too old and not maintained anymore
  * unresponsive and unstable

Knowing this, here's how we plan to achieve our three core principles.


### easy-to-use

From a user's point of view, the trace analysis core should be as easy to
use as possible.

As mentioned above, the only thing you should provide to tigerbeetle in
order to obtain a decent analysis out of it is a state provider. We won't
create and specify a fancy domain-specific language to do this: plain
Python 3 FTW.

Using Python gives you, the developer, many advantages:

  * easy to use (and learn) scripting language
  * access to the Python environment (file opening, database connections,
    math library, etc.)
  * portable state provider

Here are the expected functions a Python state provider should define:

```python
def on_init(state):
    # initialize state before receiving any event

def on_event(event, state):
    # use event and current state to modify the state

def on_fini(state):
    # modify the state one last time after having received all events
```

That's it.

We choose Python because there's already some work done in this direction
using the official LTTng trace reader,
[Babeltrace](https://lttng.org/babeltrace). However, other
languages could be supported in the future to provide states: Ruby, Lua,
Go, Node.js, etc.

If you want faster analysis, a C++11 dynamic library will also work as a
state provider, albeit more complex to write and debug.


### extensible

Message-oriented middlewares are very trendy recently and we plan to center
tigerbeetle around one of them to make it as extensible as possible (between
processes and between networked machines). [ØMQ](http://zeromq.org/) will
be tried first and kept if it proves to be a good choice.

The goal here is to create an analysis core application which will reply very
quickly to asynchronous requests received on a ØMQ socket. Here are a few
possible requests:

  * I want the event following timestamp _T_
  * I want _n_ events between timestamps _A_ and _B_
  * I want the state value of state attribute _a_ at timestamp _T_
  * I want the state values of state attributes _A_ at timestamp _T_
  * I want to know how many traces are currently being analyzed
  * I want to know the begin and end timestamps of the analyzed traces
  * etc.

Using ØMQ, any viewer/controller at any place using any language (ØMQ has
bindings for almost all modern languages) may be implemented.

Such an application is useless without other applications using its services.
The ultimate goal of all this is to facilitate the development of a very
responsive Qt 5 trace analyzer, and of a slick web equivalent using a
WebSocket-to-ØMQ bridge.


### modern

We're not afraid of using cutting edge technologies if this means faster
development and more fun to do so without trading off _too much_ performance.

We plan to use:

  * **GitHub** issues/pull requests for contributions
  * **SCons** for the build system
  * **Travis CI** for continuous integration
  * **C++11** for performance-critical applications/libraries
  * **Python 3** for pretty much everything else
  * **ØMQ 4**
  * **Qt 5**/**PyQt5** for some GUI development
  * UTF-8 **JSON-RPC** for RPCs (requests/replies and notifications)
  * **YAML** for configuration files

All those items also have the advantage of facilitating portability.
