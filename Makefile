all:
	gcc -Iinclude src/client/tftp_client.c src/common/tftp.c src/common/validation.c -o tftp_client
	gcc -Iinclude src/server/tftp_server.c src/common/tftp.c src/common/validation.c -o tftp_server

clean:
	rm -f tftp_client tftp_server

