MIDI Input Library
Copyright 2001 by Jon Parise <jparise@cmu.edu>
See COPYRIGHT for license information.

Introduction
~~~~~~~~~~~~
I needed a simple, fast MIDI input library for Win32.  A little hunting generated few exciting results, so I basically ended up writing my own.


Implementation
~~~~~~~~~~~~~~
This library is written as a Windows DLL.  Windows grants the controlling process exclusive access to the MIDI handle, and should that process exit in an unclean fashion, the MIDI handle will never be closed and will remain unavailable until the machine is rebooted.  This is obviously undesirable.

That solution was to write the code as a separate DLL, which will receive notification when the parent process terminates (cleanly or otherwise), giving it a last chance to release the MIDI handle.

The code spawns a separate worker thread to listen for MIDI input messages.  It queues these messages in a circular buffer.  This code is all hidden from you, so you really don't need to worry about it.


Examples
~~~~~~~~
At the moment, there is only one example application.  You can find it in the Test subdirectory.  It should demonstrate the complete functionality of the MIDI input library.


Contact
~~~~~~~
You can contact me at jparise@cmu.edu with questions or suggestions.