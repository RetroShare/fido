fido
====

mail gateway between the Retroshare message network and the SMTP network

Dependencies:
* Retroshare
* mimetic (http://www.codesink.org/index.html)
* A Maildir compliant SMTP server


1. Get the Retroshare sources from the Repository (0.5.5) and build it
2. clone the git repository into the plugin directory of the Retroshare sources
3. qmake && make
4. cp libFido.so.1.0.0 ~/.retroshare/extensions/Fido.so

then start Retroshare


