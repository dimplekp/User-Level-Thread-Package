# User Level Thread Package

Pre-emptive user level thread library

A file sharing system similar to BitTorrent in Java. The system uses a centralized index (CI) which has information about the files each peer the network has. Rather than using this centralized server for downloading RFCs, build a P2P-CI system in which peers who wish to download a file can download from another active peer who already has it. All communication among peers, or between a peer and the server, takes place over TCP.
