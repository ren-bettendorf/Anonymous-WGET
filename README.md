# Project2

People who worked on this:
Ren Bettendorf 831135376
Shwetha Gowdanakatte <I don't have their numbers since I forgot to ask>
Colleen DiCesare <I don't have their numbers since I forgot to ask>

The program works as expected I believe, awget expects:
./awget <url> [-c chainlist]
If no chainlist file is given it looks for chaingang.txt

ss expects like the following:
./ss [-p port]
If no port is given it attempts to bind to 9000

For the chainlist, it expects the following format:
# of stones
IP PORT
IP PORT
etc...

We don't check that these match and are expecting a valid chainlist is given.

There is a bug where if the file exists when wget command is given and goes
to a different file such as index.html exists and wget creates index.html.1
it will cause an infinite loop so try to avoid that.

It does work with concurrent calls due to using detach, but will only listen
for 5 calls so if they stack up before accept can service then they will be
refused.


