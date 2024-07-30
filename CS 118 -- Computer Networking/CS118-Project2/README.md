Members:
- Yash Shah, 405-565-567
- Nitin Subramanian, 205-607-380
- Arnav Garg, 705-484-862

High level design of router:
- The main function takes the input, extracts the LAN and WAN IPs, and then adds entries to a NAPT Table of NAPT entries that contain
  lan ip, wan ip, and lan port

- We began implementing an ACL implementation, but eventually just didn't finish it because we focused on dynamic NAPT

- Then the route function is called. It works as follows:
  - listens for packets -- the function listens to all the connections using the select() function, waiting for data to arrive from any of the connections from the passed in parameter.
  - read and validate packets -- once the select() function signals that there is data on one or more connections, the function reads the data, checks its validity by comparing computed checksums and checksum found in the header. if the checksum is invalid or TTL is 0, the pakcet is discarded
  - NAPT -- NAPT is operated on the packets that are destined to the WAN network. if the packet is intended for a destination not directly connected, the function changes the packet's source IP to the WAN IP and updates the source port to a new port
  - destination -- If the packet is from the WAN, it finds the LAN IP and port associated with the WAN port from the NAPT table and sets that as the destination. If the packet is destined for a known LAN IP, it sets that as the destination.
  - forwarding -- function sends the packet to its intended destination.

  We had several issues that took us some time. We were having issues with the static NAPT, due to a lot of our header calculation and checksum calculation being off. Eventually, with enough print statements to identify the source of the problem and continuous evaluation of how the packets worked, we were able to solve it. This led to other vulnerabilities in how we were computing the header lengths, source IP, source ports, and such, which we were able to eventually hash out. Then we had some issues on the dynamic NAPT -- we realized that only LAN -> WAN required such operations and we were breaking too early from the loop. Once we had resoluted that, we were good to go to submit.

  We took inspiration from a few resources. They are listed above the functions we used them for.