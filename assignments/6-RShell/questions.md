1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The remote client determines a command's output is complete once it receives a EOF character from the server. This EOF marks the ends of the message and stops the loop. We use a loop to handle partial reads until the complete message is receievd.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

The network shell protocols should add a marker such as null terminator or EOF character at the end of the message to mark the end of message. If not handled properly, then parts of messages or commands may get lost or mixed up, causing errors.

3. Describe the general differences between stateful and stateless protocols.

The general difference is that stateful protocol remembers the client's session while stateless doesn't. This makes stateful protocols complex.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

Although UDP is unreliable, it would be used because it has lower latency and overhead compared to TCP. It would be used when speed is more needed.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

OS provides sockets to allow applications to use network communications.