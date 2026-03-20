#!/usr/bin/env python3
import sys
import time

def main():
	# Proper HTTP headers with blank line separator
	sys.stdout.write("Content-Type: text/plain\r\n")
	sys.stdout.write("Content-Length: 14\r\n")
	sys.stdout.write("\r\n")
	sys.stdout.flush()
	
	time.sleep(3)
	
	sys.stdout.write("slow cgi done\n")
	sys.stdout.flush()

if __name__ == "__main__":
	main()
