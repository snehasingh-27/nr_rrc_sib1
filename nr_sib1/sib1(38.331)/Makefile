CC = gcc
CFLAGS = -Wall -Wextra -I. -DASN_DISABLE_OER_SUPPORT
LDFLAGS = -lm

# Automatically find all generated .c files
ASN_SOURCES = $(wildcard *.c)
ASN_OBJECTS = $(filter-out encoder.o decoder.o, $(ASN_SOURCES:.c=.o))

all: encoder decoder

encoder: encoder.o $(ASN_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

decoder: decoder.o $(ASN_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o encoder decoder

run-test: encoder decoder
	@echo "Starting decoder (in background)..."
	./decoder &
	@sleep 1
	@echo "Running encoder..."
	./encoder
	@sleep 1
	@echo "Test complete!"

.PHONY: all clean run-test
