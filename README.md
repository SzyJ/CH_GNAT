![GNAT_Banner](Images/GNAT_Banner.jpg)
## To clone
In order to also clone the 3rd party logging library, please remember to update submodules. Without this, the code will not compile.
```
git clone https://github.com/SzyJ/GNAT_Core
git submodule init
git submodule update
```


# GNAT-Core Behaviours
Server:
 - [X] Wait for X amount of clients to send msg, save their IP
 - [ ] Gain Ping of each client
 - [X] Start Loop that brodcasts state every X Miliseconds

Client:
 - [X] Send Join Message to server
 - [ ] Respond to Ping request
 - [X] Receive real state from server and update local values 
 - [X] Listen for input from terminal. Send this update to Server
 
Peer (Session-Host):
 - [ ] Listen on one port for X Join Requests
 - [ ] Boradcast IP of each Peer along with their starting value

Peer (Normal):
 - [ ] Send Join Request
 - [ ] Listen for info about each Peer, update local knowledge

Peer:
 - [ ] Receive from each client on a different port
 - [ ] Listen for terminal input and broadcast this value to all other peers
