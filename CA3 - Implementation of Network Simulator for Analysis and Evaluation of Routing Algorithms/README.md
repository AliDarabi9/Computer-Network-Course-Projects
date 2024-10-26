 # Ali Darabi - 810100264

# Hesam Ramezanian - 810100248

---

### main.cpp

This file contains the core functionality for initializing and managing the nodes and routers in the network simulation. The key components include:

- **Includes and Constants**: Imports necessary Qt libraries and defines constants for the counts of routers and PCs in two Autonomous Systems (AS).
- **Functions**:
  - `createThreadsForNodes()`: Moves each node to a separate thread and starts the threads.
  - `update()`: Calls the `update` method on all nodes.
  - `bindNodes()`: Connects two nodes by binding their ports.
  - `parseLine()`, `findByID()`, `bindByInputLine()`, `createTopology()`: Functions for reading and processing the network topology from a file.
  - `initializeRouters()`, `initializePCs()`: Initializes router and PC nodes.
  - `setupApplicationCleanup()`: Ensures cleanup of threads and nodes when the application quits.
  - `ripRun()`, `setupRipRun()`, `runOSPFOnNodesSequentially()`: Functions to start RIP and OSPF protocols on nodes.
  - `finishInASProtocols()`, `sendEBGP()`, `sendIBGP()`, `sendFromPC()`: Handles the completion of protocols and starts inter-AS communication.
  - `generatePoissonNumbers()`, `countOccurrences()`: Generates packet counts based on Poisson distribution.
  - `printRouterTables()`: Prints routing tables for debugging.
  - `finishProtocolTimer()`: Manages the transition between different protocol phases.
  - `getInputProtocolTypes()`: Reads the protocol types for the ASes from user input.
- **main()**: The entry point of the program. Initializes nodes and threads, sets up protocols, and manages the overall simulation lifecycle.

---

### node.h

This header file defines the `node` class, which serves as a base class for all network nodes in the simulation. The key components include:

- **Includes and Constants**: Includes necessary Qt and project-specific headers, and defines constants for different protocol types (OSPF, RIP, etc.).
- **Class Definition**: 
  - `node` is an abstract class derived from `QObject`.
  - **Constructor**: Initializes the node and assigns a unique ID.
  - **Public Methods**:
    - `bindDestPort()`, `addNewPort()`, `getPortByPortNumber()`: Pure virtual functions to manage ports, meant to be implemented by derived classes.
    - `getID()`: Returns the node's unique ID.
  - **Public Slots**: 
    - `update()`, `startProtocol()`, `printTable()`, `startEBGP()`, `turnOffProtocols()`, `startIBGP()`, `startPCs()`: Virtual slots for updating the node, starting protocols, printing routing tables, and managing inter-node communication.
  - **Protected Members**: 
    - `ID`: Static member to track the global node ID.
    - `_id`: Unique ID for the current node.
    - `asID`: Autonomous System ID.
    - `IP`: Node's IP address.
  - **Signals**: 
    - `OSPFFinished()`, `RIPFinished()`: Signals to indicate the completion of OSPF and RIP protocols.

### node.cpp

This source file provides the implementation for the `node` class defined in `node.h`. The key components include:

- **Constructor**: 
  - Initializes the node with a unique ID incremented from a static member.
- **Methods**:
  - `getID()`: Returns the unique ID of the node.
  - `startProtocol()`, `printTable()`, `startEBGP()`, `turnOffProtocols()`, `startIBGP()`, `startPCs()`: Placeholder implementations for the respective functions, to be overridden by derived classes.

This class provides the foundational structure for all types of nodes in the network simulation, with key functionalities to be extended by derived classes.

---

### router.h

This header file defines the `Router` class, which is derived from the `node` class and implements a network router. The key components include:

- **Includes and Class Declaration**: Includes necessary headers and declares the `Router` class as a subclass of `node`.
- **Public Methods**:
  - **Constructor**: Initializes the router with a specified protocol and autonomous system ID.
  - **addNewPort()**: Adds a new port to the router.
  - **bindDestPort()**: Binds a destination port to a specific port number.
  - **getPortByPortNumber()**: Retrieves a port based on its port number.
- **Public Slots**:
  - **update()**: Updates the router's state.
  - **startProtocol()**: Starts the specified protocol (OSPF or RIP) if the router's AS ID matches.
  - **printTable()**: Prints the router's forwarding table.
  - **startEBGP()**: Starts the EBGP protocol.
  - **turnOffProtocols()**: Turns off all protocols.
  - **startIBGP()**: Starts the IBGP protocol.
- **Private Members**:
  - **ports**: A vector of pointers to ports.
  - **forwardingTable**: A hash table for storing forwarding information.
  - **send()**: Sends data through a specified port.
  - **forward()**: Forwards data to the next hop.
  - **lsdb**: Link State Database for OSPF.
  - **distanceVector**: Distance vector table for RIP.
  - **updateLsp()**: Updates the link state database.
  - **sendLsp()**: Sends a link state packet.
  - **lspBackTrack()**: Handles backtracking in OSPF.
  - **isStartOSPF**: Flag indicating if OSPF has started.
  - **lspIndex**: Index for traversing ports during OSPF.
  - **lspReceivePort**: Port for receiving LSPs.
  - **sendOSPF**: Flag indicating if an OSPF packet should be sent.
  - **isNodeVisited()**: Checks if a node has been visited in OSPF.
  - **startOSPF()**: Starts the OSPF protocol.
  - **updateOSPF()**: Updates OSPF state.
  - **ProtocolType**: Type of protocol being used (OSPF or RIP).
  - **updateRIP()**: Updates the RIP state.
  - **updateDistanceVector()**: Updates the distance vector table.
  - **ripFinished**: Flag indicating if RIP has finished.
  - **sendRip()**: Sends a RIP packet.
  - **startRIP()**: Starts the RIP protocol.
  - **ripConvergenceCondition**: Condition for RIP convergence.
  - **ripFinishSignalEmited**: Flag indicating if RIP finish signal has been emitted.
  - **isRIPstart**: Flag indicating if RIP has started.
  - **isOSPFstart**: Flag indicating if OSPF has started.
  - **handleOSPFFinish()**: Handles the completion of OSPF.
  - **updateForwarding()**: Updates forwarding based on received data.
  - **iseBGP**: Flag indicating if EBGP is used.
  - **increaseTimeInBuffer()**: Increases the time in the buffer for packets.

### router.cpp

This source file provides the implementation for the `Router` class defined in `router.h`. The key components include:

- **Constructor**: Initializes the router with a unique ID, protocol type, AS ID, and a default IP address.
- **send()**: Sends data through a specified port, prepending a message type if needed.
- **addNewPort()**: Adds a new port to the router.
- **bindDestPort()**: Binds a destination port to a specific port number and updates the link state and distance vector tables.
- **startOSPF()**: Initializes and starts the OSPF protocol.
- **startRIP()**: Initializes and starts the RIP protocol.
- **startProtocol()**: Starts the specified protocol if the router's AS ID matches.
- **isNodeVisited()**: Checks if a node has been visited in OSPF.
- **sendLsp()**: Sends a link state packet to the next port in the list.
- **getPortByPortNumber()**: Retrieves a port based on its port number.
- **forward()**: Forwards a packet to the next hop based on the forwarding table.
- **updateLsp()**: Updates the link state database with new data.
- **lspBackTrack()**: Handles backtracking in OSPF.
- **handleOSPFFinish()**: Handles the completion of OSPF and emits the OSPFFinished signal.
- **updateOSPF()**: Updates the OSPF state by processing received packets and sending LSPs.
- **updateDistanceVector()**: Updates the distance vector table based on new data.
- **sendRip()**: Sends a RIP packet to all ports within the same AS.
- **updateRIP()**: Updates the RIP state by processing received packets and checking for convergence.
- **printTable()**: Prints the router's forwarding table and costs.
- **increaseTimeInBuffer()**: Increases the time in the buffer for packets.
- **update()**: Updates the router's state based on the protocol in use and handles forwarding.
- **updateForwarding()**: Updates forwarding based on received data.
- **startEBGP()**: Starts the EBGP protocol by sending distance vector data to external peers.
- **turnOffProtocols()**: Turns off all protocols and updates the distance vector table with link state data if OSPF was used.
- **startIBGP()**: Starts the IBGP protocol and updates the forwarding table based on the distance vector table.

This class implements the core functionalities for a router in a network simulation, including OSPF and RIP protocols, as well as forwarding and inter-domain routing capabilities.

---

### pc.h

This header file defines the `pc` class, which represents a personal computer (PC) in the network simulation. The key components include:

- **Includes and Class Declaration**:
  - Includes necessary headers (`QObject`, `QThread`, and `node.h`).
  - Declares the `pc` class as a subclass of `node`.

- **Public Methods**:
  - **Constructor**: Initializes the PC with an autonomous system ID (`idAS`).
  - **bindDestPort()**: Binds a destination port to a specified port number.
  - **addNewPort()**: Adds a new port to the PC.
  - **getPortByPortNumber()**: Retrieves a port based on its port number.

- **Public Slots**:
  - **startPCs()**: Starts communication by sending a packet to a destination ID.
  - **update()**: Updates the PC's state by checking and processing incoming data on its port.

- **Private Members**:
  - **sourcePort**: A pointer to the PC's port.

### pc.cpp

This source file provides the implementation for the `pc` class defined in `pc.h`. The key components include:

- **Constructor**:
  - Initializes the PC with a unique ID, AS ID, and a default IP address.
  - Sets the PC's AS ID (`asID`).
  - Generates the PC's IP address based on its ID.

- **addNewPort()**:
  - Creates a new `port` object with the specified port number, PC ID, and AS ID.
  - Assigns this new port to `sourcePort`.

- **bindDestPort()**:
  - Sets the destination port for `sourcePort` to `destPort`.

- **getPortByPortNumber()**:
  - Returns the `sourcePort`.

- **startPCs()**:
  - Creates a `packet` object with the message "HELLO", source ID, and destination ID.
  - Sends the encoded packet through `sourcePort`.

- **update()**:
  - Checks if `sourcePort` has any data in its buffer.
  - If data is available, it receives and decodes the data into a `packet` object.
  - Prints the received packet's details using `qDebug()`.
  - Deletes the packet object to free memory.

---

### port.h

The `port` class represents a network port used for communication between nodes in a network simulation. It inherits from `QObject` and includes several public methods, a constructor, and member variables.

**Header File Structure**:
- **Public Methods**:
  - Constructor: Initializes the port with a given number, node ID, and autonomous system ID.
  - `send`: Sends data to the destination port.
  - `receive`: Receives data from the port's buffer.
  - `setPortDest`: Sets the destination port for sending data.
  - `isPortNumber`: Checks if the port number matches a given number.
  - `isBufferEmpty`: Checks if the buffer is empty.
  - `getNodeID`: Returns the node ID associated with the port.
  - `getPortNumber`: Returns the port number.
- **Public Members**:
  - `destination`: Pointer to the destination port.
  - `asID`: Autonomous system ID.
  - `buffer`: Buffer for storing data to be sent or received.
- **Private Members**:
  - `portNumber`: Port number.
  - `_nodeID`: Node ID.

### port.cpp

**Implementation File Structure**:
- Constructor initializes the port with specified number, node ID, and autonomous system ID.
- `getNodeID` returns the node ID.
- `send` appends data to the destination port's buffer.
- `getPortNumber` returns the port number.
- `receive` retrieves and removes the first data entry from the buffer.
- `setPortDest` sets the destination port.
- `isPortNumber` checks if the given number matches the port number.
- `isBufferEmpty` checks if the buffer is empty.

---

### packet.h

The `packet` class represents a network packet used for communication in a network simulation. It inherits from `QObject` and includes methods for encoding, decoding, and managing packet data.

**Header File Structure**:
- **Public Methods**:
  - Constructor: Initializes the packet with a message, source ID, and destination ID.
  - `decode`: Static method to create a packet from a QString representation.
  - `incode`: Static method to convert a packet to a QString representation.
  - `addRouterToPath`: Adds a router ID to the path of the packet.
  - `printPacket`: Returns a QString representation of the packet's information.
  - `increaseBufferTime`: Increases the cycle count of the packet in the buffer.
- **Public Members**:
  - `message`: The message contained in the packet.
  - `sourceID`: The source node ID.
  - `sourceIP`: The source IP address.
  - `destID`: The destination node ID.
  - `destIP`: The destination IP address.
  - `routersInPath`: List of routers the packet has passed through.
  - `cycleInBuffer`: The number of cycles the packet has spent in the buffer.

### packet.cpp

**Implementation File Structure**:
- Constructor initializes the packet with the specified message, source ID, and destination ID, and sets the IP addresses.
- `increaseBufferTime` increments the cycle count of the packet in the buffer.
- `decode` creates a new packet from a QString representation by parsing the components.
- `addRouterToPath` adds a router ID to the path list.
- `incode` converts the packet to a QString representation by concatenating its components.
- `printPacket` generates a QString representation of the packet's information, including source and destination IPs, message, routers in the path, path length, and cycles in buffer. 

---

### lsp.h

The `lsp` class represents a Link State Packet (LSP) used in network protocols. It inherits from `QObject` and includes methods for encoding and decoding LSPs.

**Header File Structure**:
- **Public Methods**:
  - Constructor: Initializes the LSP object.
  - `decode`: Static method to create an LSP from a QString representation.
  - `incode`: Static method to convert an LSP to a QString representation.
- **Public Members**:
  - `table`: QHash mapping node IDs to costs.
  - `visitedNodeIDs`: QVector of node IDs that have been visited.

### lsp.cpp

**Implementation File Structure**:
- Constructor initializes the LSP object.
- `decode` creates a new LSP from a QString representation by parsing the components.
- `incode` converts the LSP to a QString representation by concatenating its components.

---

### rip.h

The `rip` class represents a Routing Information Protocol (RIP) packet used in network protocols. It inherits from `QObject` and includes methods for encoding and decoding RIP packets.

**Header File Structure**:
- **Public Methods**:
  - Constructor: Initializes the RIP object with an optional parent QObject.
  - `decode`: Static method to create a RIP object from a QString representation.
  - `incode`: Static method to convert a RIP object to a QString representation.
- **Public Members**:
  - `table`: QHash mapping node IDs to costs.

### rip.cpp

**Implementation File Structure**:
- Constructor initializes the RIP object.
- `decode` creates a new RIP object from a QString representation by parsing the components into key-value pairs and storing them in the `table` member.
- `incode` converts the RIP object to a QString representation by concatenating the key-value pairs from the `table` member.
  
---
