bin/netcp: src/netcp.h src/netcp.c
	@mkdir -p bin
	gcc src/netcp.c \
		-o bin/netcp \
		-Wall

clean:
	@rm bin/netcp
	@rm -r bin
