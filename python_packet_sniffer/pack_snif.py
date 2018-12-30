import socket
import struct
import textwrap

def main():
	conn = socket.socket(socket.AF_PACKET, socket.SOCK_RAW, socket.ntohs(3))
	while True:
		raw_data, addr = conn.recvfrom(65536)
		receiver_mac, sender_mac, eth_prot, data = ethernet_frame(raw_data)
		print('\nEthernet Frame:')
		print('\tReceiver: {}, Sender: {}, Protocol: {}'.format(receiver_mac, sender_mac, eth_prot))
		
		if eth_prot == 8:
			(version, header_length, ttl, proto, src, des, data) = ipv4_packet(data)
			print('\tIPv4 Packet:')
			print('\t\tVersion : {}, Header Length: {}, TTL: {}'.format(version, header_length, ttl))
			print('\t\tProtocol: {}, Src: {} , Des: {}'.format(proto, src, des))
			
			# ICMP
			if proto == 1:
				icmp_type, code, checksum, data = icmp_packet(data)
				print('\tICMP Paclet:')
				print('\t\tType: {}, Code: {} , Checksum: {}'.format(icmp_type, code, checksum))
				print('\t\tData:')
				print(format_multiline('\t\t\t', data))

			# TCP
			if proto == 6:
				(src_port, des_port, sequence, ack, flag_urg, flag_ack, flag_psh, flag_rst, flag_syn, flag_fin) = tcp_packet(data)
				print('\tTCP Segment:')
				print('\t\tSrc Port: {} , Des Port: {}'.format(src_port, des_port))
				print('\t\tSequence: {}, Ack: {}'.format(sequence, ack))
				print('\t\tFlags:')
				print('\t\t\tURG: {}, ACK: {}, PSH: {}, RST: {}, SYN: {}, FIN: {}'.format(flag_urg, flag_ack, flag_psh, flag_rst, flag_syn, flag_fin))
				print('\t\tData:')
				print(format_multiline('\t\t\t', data))

			# UDP
			if proto == 17:
				src_port, des_port, length, data = udp_packet(data)
				print('\tUDP Segment:')
				print('\t\tSrc Port: {}, Des Port: {}, Length: {}'.format(src_port, des_port, length))

			# Other
			else:
				print('\tData:')
				print(format_multiline('\t\t', data))	

# Unpack ethernet frame
# | SYNC 8 | RECEIVER 6 | SENDER 6 | TYPE 2 | PAYLOAD 46-1500 | CRC 4 |
def ethernet_frame(data):
	receiver_mac, sender_mac, protocol = struct.unpack('! 6s 6s H', data[:14])
	return mac_to_str(receiver_mac), mac_to_str(sender_mac), socket.htons(protocol), data[14:]

# MAC address to String
def mac_to_str(mac_addr):
	bytes_str = map('{:02x}'.format, mac_addr)
	return ':'.join(bytes_str).upper()

# Unpack IPv4 packet
# | Version 1/2 | IHL(Header Length) 1/2 | Type of Service (TOS) 1 | Total Length 2 			| 
# | Identification	2	          			   | IP Flags 3/8  Fragment offset 	|
# | Time to Live (TTL) 1 		 | Protocol 1		   | Header Checksum			|
# |		Source Addres 4										|
# | 		Destination Address 4									|
# | 		IP Option (optional)	4								|
def ipv4_packet(data):
	version = data[0] >> 4
	header_length = (data[0] & 15) * 4
	ttl, protocol, src, des = struct.unpack('! 8x B B 2x 4s 4s', data[:20])
	return version, header_length, ttl, protocol, ipv4(src), ipv4(des), data[header_length:]

# Return formatted IPv4 address
def ipv4(addr):
	return '.'.join(map(str, addr))

# Unpack ICMP paccket
def icmp_packet(data):
	icmp_type, code, checksum = struct.unpack('! B B H', data[:4])
	return icmp_type, code, checksum, data[4:]

# Unpack TCP packet
def tcp_packet(data):
	(src_port, des_port, sequence, ack, offset_reserved_flags) = struct.unpack('! H H L L H', data[:14])
	offset = (offset_reserved_flags >> 12)  * 4
	flag_urg = (offset_reserved_flags & 32)  >> 5
	flag_ack = (offset_reserved_flags & 16)  >> 4
	flag_psh = (offset_reserved_flags & 8)  >> 3
	flag_rst = (offset_reserved_flags & 4)  >> 2
	flag_syn = (offset_reserved_flags & 2)  >> 1
	flag_fin = (offset_reserved_flags & 1)
	return src_port, des_port, sequence, ack, flag_urg, flag_ack, flag_psh, flag_rst, flag_syn, flag_fin

# Unpack UDP packet
def udp_packet(data):
	src_port, des_port, size = struct.unpack('! H H 2x H', data[:8])
	return src_port, des_port, size, data[8:]

# Format multiline data
def format_multiline(prefix, string, size=80):
	size -= len(prefix)
	if (isinstance(string, bytes)):
		string = ''.join(r'\x{:02x}'.format(byte) for byte in string)
		if size % 2:
			size -= 1
	return '\n'.join([prefix + line for line in textwrap.wrap(string, size)])




print("------------Packet Sniffer-------------")
main()
