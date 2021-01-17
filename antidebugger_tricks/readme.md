# At the start of the program the first instruction is to see if a debugger is present. Then once the counter hits 30 it will launch a thread that will continue to search (Every 2 seconds) for an attached debugger.

* Checks if process was started with a debugger.
* Checks if a debugger has been attached, around every 4 seconds.
