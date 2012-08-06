#libpastebin#

libpastebin contains the api for [pastebin.com](http://pastebin.com/) written in C.

Dependencies are simple for the library, since only libcurl is needed.

Installation _should_ be as simple as:
```
sudo apt-get install libcurl4-openssl-dev
```

== Compiling ==

The makefile is pretty braindead and only builds 3 things:
* libpastebin -- Just the file libpastebin.so, in the lib directory
* pb -- a self-contained executable that allows you to paste files to pastebin. Look at ```bin/pb --help``` for more information. This is the default for make all
* pb_debug -- Same as above, with a **lot** of debugging output.
