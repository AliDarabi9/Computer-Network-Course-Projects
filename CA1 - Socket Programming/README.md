**Explain Code:**

**1-audioplay:** 

**Class Members:**

QVector<QBuffer*> sourceBuffer: Holds the audio buffers to be played.

QBuffer* buff: Pointer to the current buffer being played.

QAudioFormat format: Specifies the audio format (8000 Hz, 1 channel, 8-bit unsigned integer).

QAudioSink* audio: Handles the audio playback.

**Constructor**
The constructor initializes sourceBuffer with the provided QVector<QBuffer*> and sets up the format for audio playback.

**run()**
This function starts the audio playback by:
Checking if there are buffers available and no buffer is currently being played.
Retrieving the first buffer from sourceBuffer.
Opening the buffer for reading.
Creating a QAudioSink object with the specified format.
Starting to play the buffer.

**handleStateChanged()**
This slot function is connected to QAudioSink's stateChanged signal. It stops the audio output when the IdleState is reached or handles errors if the StoppedState is encountered with an error.

**stopAudioOutput()**
This function stops the audio playback by stopping the QAudioSink object, closing the current buffer, and cleaning up resources.



**2-audioRec:**

**Class Members:**

bool isMute: A flag indicating whether the audio recording is muted or not.

QVector<QBuffer*> destinationBuffer: A vector to store the recorded audio buffers.

QBuffer* buff: A pointer to the current buffer being recorded.

QAudioFormat format: The audio format used for recording, set to 8000 Hz sample rate, 1 channel, and 8-bit unsigned integer sample format.

QAudioSource* audio: A pointer to the QAudioSource object responsible for recording the audio data.

**Constructor**

The constructor takes a QVector<QBuffer*> as a destination for the recorded buffers and an optional QObject parent pointer. It initializes the destinationBuffer member variable with the provided vector and sets up the format for audio recording.

**run():** 

This function starts the audio recording process. It creates a new QBuffer object, opens it for writing, creates a QAudioSource object with the specified format, connects the stateChanged signal of QAudioSource to the handleStateChanged slot, and starts the recording process unless isMute is true. Additionally, it sets a single-shot QTimer to stop the recording after 2.5 seconds.

**handleStateChanged(QAudio::State newState):** 

This slot function is connected to the stateChanged signal of the QAudioSource object. It handles the different states of the audio recording, currently only handling the StoppedState for error cases.

**stopRecording():** 

This function stops the audio recording by stopping the QAudioSource object (if not muted), adding the recorded buffer to the destinationBuffer vector, cleaning up resources, and emitting the finished signal.

**toggleMuteState():** 

This function toggles the isMute flag and prints the current mute state to the debug console.



**3-receive:**

**Class Members:**

QVector<QBuffer*> reciveBuffer_: A vector to store the received data buffers.

boost::asio::ip::tcp::socket* socket_: A pointer to the Boost.Asio TCP socket used for receiving data.

boost::asio::ip::tcp::acceptor* acceptor_: A pointer to the Boost.Asio TCP acceptor used for accepting incoming connections.

**Constructor**

The constructor takes an int representing the port number to listen on, a QVector<QBuffer*> as the destination for the received buffers, and an optional QObject parent pointer. It initializes the following:
reciveBuffer_ with the provided vector.
A new boost::asio::io_service object.
A new boost::asio::ip::tcp::socket object using the io_service.
A new boost::asio::ip::tcp::acceptor object using the io_service and the specified port number.
Accepts an incoming connection on the acceptor_ and assigns it to the socket_.

**void Start():** 

This function is responsible for receiving data over the TCP connection. It uses the boost::asio::read_until function to read data from the socket_ until the newline character ('\n') is encountered. If the received data is not empty, it creates a new QBuffer object, opens it for writing, writes the received data to the buffer, closes the buffer, and adds it to the reciveBuffer_ vector. Finally, it sets a single-shot QTimer to call Start() again after 100 milliseconds, effectively creating a loop for continuous data reception.



**4-send:**

**Class Members:**

QVector<QBuffer*> packetBuff_: A vector containing the data buffers to be sent.

boost::asio::io_service* io_service_: A pointer to the Boost.Asio io_service object.

boost::asio::ip::tcp::socket* socket_: A pointer to the Boost.Asio TCP socket used for sending data.

std::string ip_: The IP address to which the data will be sent.

**Constructor**

The constructor takes an int representing the port number, a QVector<QBuffer*> containing the data buffers to be sent, a const std::string& representing the IP address, and an optional QObject parent pointer. It initializes the following:
packetBuff_ with the provided vector of buffers.
A new boost::asio::io_service object.
A new boost::asio::ip::tcp::socket object using the io_service_.
Attempts to connect the socket_ to the specified IP address and port for up to 100 times, sleeping for 1 second between attempts if the connection fails. If the connection fails after 100 attempts, it prints a "Connection Time Out!" message to the debug console.
Assigns the provided IP address to the ip_ member variable.

**void startSending():** 

This function is responsible for sending data over the TCP connection. It retrieves the first buffer from the packetBuff_ vector, converts its data to a QByteArray, and sends the data over the socket_ using the boost::asio::write function. If the packetBuff_ is empty, it sends an empty string. After sending the data, it appends a newline character ('\n') to the sent data.


**5-main:**

The `main` function is the entry point of the application and performs the following tasks:

1. Creates a `QApplication` instance.
2. Initializes `QVector` objects (`packetBuffIn` and `packetBuffOut`) to store the incoming and outgoing audio buffers, respectively.
3. Prompts the user to enter the send and receive ports for the TCP connection.
4. Creates instances of the `audioRec`, `audioplay`, `send`, and `receive` classes based on the provided port numbers and user role (sender or receiver).
5. Connects signals and slots for the `audioRec`, `audioplay`, and `send` classes to handle audio recording, playback, and data sending.
6. Starts the audio recording and playback processes, as well as the data sending and receiving processes.
7. Creates a `QWidget` with a `QPushButton` to toggle the mute state of the audio recording.
8. Shows the `QWidget` and enters the application's event loop.

### `audioRec`

- Responsible for recording audio data and storing it in the `packetBuffOut` vector.
- The `finished` signal is connected to the `run` slot to continuously record audio data.
- The `toggleMuteState` slot is connected to a `QPushButton` to toggle the mute state of the audio recording.

### `audioplay`

- Responsible for playing audio data from the `packetBuffIn` vector.
- The `finished` signal is connected to the `run` slot to continuously play audio data.

### `send`

- Responsible for sending audio data from the `packetBuffOut` vector over a TCP connection.
- The `startSending` slot is connected to a `QTimer` to periodically send audio data.

### `receive`

- Responsible for receiving audio data over a TCP connection and storing it in the `packetBuffIn` vector.
- The `Start` function is called after a short delay to initiate the data reception process.



////////////////////////////////////////////////////////////////////////////////////////////////////



**WebRTC Report:**

WebRTC (Web Real-Time Communication) is an open-source project that lets web browsers and mobile apps talk to each other in real-time. It helps people share audio, video, and data directly, without needing special plugins or extra software from other companies.

**Positive Aspects of WebRTC:**
1. No Plugin Required: Modern web browsers already have WebRTC built-in, so you don't have to download any special programs or add-ons.
2. Peer-to-Peer Communication: With WebRTC, you can connect with others right from your browser, without needing a server in the middle. This can make things faster and smoother.
3. High-Quality Media: WebRTC lets you have high-quality video and audio chats, perfect for things like video conferencing, live streams, and working together online.
4. Cross-Platform Compatibility:  WebRTC works on all sorts of devices and operating systems, so everyone can join the conversation no matter what they're using.
5. Security: WebRTC uses scrambled messages to keep your conversations private and secure.

**Negative Aspects of WebRTC:**
1. Network Limitations: WebRTC works best when devices can connect directly, like two friends talking on the phone. But sometimes, things get in the way like strict security guards (firewalls) or confusing network setups (NAT). These can make those direct connections difficult. Luckily, WebRTC has tools to deal with these situations, but it's something to keep in mind.
2. Scalability Challenges: Getting WebRTC to work for a lot of people at the same time (large-scale applications) can be a bit tricky. It might also require some extra servers (server infrastructure) to handle everything smoothly.
3. Learning Curve: Creating WebRTC applications can present a significant learning curve, particularly for developers who are new to real-time communication technologies.

**STUN (Session Traversal Utilities for NAT):** 
Imagine two friends, Ali and Hesam, want to have a video call using WebRTC. However, they both are behind routers or firewalls, which act like gates that control the traffic going in and out of their networks. These gates can sometimes make it difficult for Ali and Hesam to directly connect with each other.
This is where STUN comes into play. STUN acts like a friendly neighbor who can help Ali and Hesam figure out how to connect with each other.
1.	Ali sends a message to the STUN server, asking, "what's my public address?"
2.	The STUN server looks at the source address of Ali's message and replies with something like, "Your public address is 125.125.125.125."
3.	Similarly, Hesam also contacts the STUN server and gets his public address.
Now, Ali and Hesam know each other's public addresses, which helps them establish a direct connection for their video call. If they can't connect directly due to strict firewalls or routers, the STUN server can also tell them the type of NAT (Network Address Translation) they are behind, which helps them find alternative ways to connect.


**TURN (Traversal Using Relays around NAT):**
let's assume that despite the help from STUN, Ali and Hesam still can't establish a direct peer-to-peer connection due to very strict firewalls or symmetric NAT configurations. This is where TURN comes into play.
Think of TURN as a friendly relay service that can help Ali and Hesam communicate when a direct connection is not possible.
Here's how it works:
1.	Ali tries to connect directly to Hesam but fails due to the strict firewall or NAT restrictions.
2.	Ali then contacts a TURN server and says, " I can't seem to connect to Hesam directly."
3.	The TURN server responds, " You can send your data to me, and I'll relay it to Hesam."
4.	Similarly, Hesam also contacts the TURN server and sets up a relay.
5.	Now, instead of trying to connect directly, Ali sends her video and audio data to the TURN server, which then forwards it to Hesam, and vice versa.
Essentially, the TURN server acts as an intermediary or relay point, facilitating the exchange of media data between Ali and Hesam when a direct peer-to-peer connection is not possible.
It's like having a friendly neighbor who agrees to pass messages back and forth between Ali and Hesam when they can't directly communicate with each other due to obstacles like strict firewalls or complex networking setups.
While using a TURN server can introduce some additional latency and overhead, it provides a reliable fallback solution for WebRTC communication when direct peer-to-peer connections fail.



**Signaling Server:**
Imagine Ali and Hesam want to have a video call using WebRTC, but they don't know each other's contact information or how to initiate the call. This is where the Signaling Server comes into play. It acts as a common point of contact, helping Ali and Hesam establish a connection.
Think of the Signaling Server as a friendly receptionist or coordinator who helps facilitate the initial setup of the video call between Ali and Hesam.
Here's how it works:
1. Ali contacts the Signaling Server and says, " I want to have a video call with Hesam."
2. The Signaling Server responds, " Let me know when Hesam is also ready to connect."
3. When Hesam is ready, he also contacts the Signaling Server and says, "I'm ready for the video call with Ali."
4. The Signaling Server then exchanges the necessary information between Ali and Hesam, such as their IP addresses, port numbers, and other details required to establish a peer-to-peer connection.
5. With the information provided by the Signaling Server, Ali and Hesam can now initiate a direct peer-to-peer connection for their video call.
The Signaling Server doesn't handle the actual media data (video, audio, or data) exchanged during the call. It's solely responsible for coordinating the initial connection setup and facilitating the exchange of necessary information between the WebRTC clients (Ali and Hesam).
It's like having a friendly receptionist who helps two people coordinate a meeting by exchanging their contact details and scheduling information, but doesn't participate in the actual meeting itself.
The Signaling Server is a crucial component in WebRTC because it allows clients to locate each other and negotiate the parameters for establishing a direct peer-to-peer connection, even if they don't have each other's contact information beforehand.
