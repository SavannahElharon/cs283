1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The remote client determines that a command’s output is fully received by either detecting an agreed-upon end-of-message delimiter or closing the connection if that signals completion. To handle partial reads, techniques like buffering and checking for termination sequences can be used. Another approach is to prefix messages with their length so the client knows how much data to expect.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

A networked shell protocol can define message boundaries by using delimiters or length prefixing to indicate how much data will be sent. Without these, the client may receive fragmented messages due to TCP’s stream oriented nature, making it difficult to parse commands correctly. If not handled properly the protocol could misinterpret data leading to corrupted commands, incomplete processing, or security vulnerabilities.

3. Describe the general differences between stateful and stateless protocols.

A stateful protocol maintains session information between requests allowing it to track previous interactions and provide context aware responses. A stateless protocol, on the other hand, treats each request independently with no memory of past exchanges, making it simpler and more scalable. Stateful protocols generally offer more reliability and continuity, while stateless ones tend to be more efficient for high throughput applications.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

We use UDP despite its unreliability because it has lower overhead and is much faster than TCP making it ideal for applications that prioritize speed over other things. It is commonly used for streaming, gaming, and where packet loss is acceptable and retransmitting lost data would cause unnecessary delays. UDP is also useful in cases where the application itself handles reliability instead of relying on the transport layer.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The operating system provides the socket API as the primary interface for network communication allowing applications to send and receive data over the network. This interface supports both connection oriented (TCP) and connectionless (UDP) communication, which is more flexible for different networking needs.
