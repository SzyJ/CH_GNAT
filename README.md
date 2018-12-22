# CH_GNAT
Client Hosted Model - Game Networking API Template

# GNAT-Core Behaviours
Server:
 - Wait for X amount of clients to send msg, save their IP
 - Gain Ping of each client
 - Start Loop that brodcasts state every X Miliseconds

Client:
- Send Join Message to server
- Respond to Ping request
- Receive real state from server and update local values 
- Listen for input from terminal. Send this update to Server

Peer:
- Receive from each client on a different port
- Listen for terminal input and broadcast this value to all other peers
