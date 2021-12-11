CC=gcc
CFLAGS=-Wall -g
AISLE_FILES = aisle_manager.o .aisle_util.o store_util.o aisle_test.c
STORE_FILES = aisle_manager.o .aisle_util.o .store_util.o store_util.o store_client.c store_test.c

all: store_test aisle_test

aisle_manager.o: aisle_manager.c
	@echo "Building aisle_manager.c"
	@$(CC) $(CFLAGS) -c $< -o $@

store_util.o: store_util.c
	@$(CC) $(CFLAGS) -c $< -o $@

store_test: $(STORE_FILES)
	@echo "Building store_test"
	@$(CC) $(CFLAGS) -Wconversion $(STORE_FILES) -o $@

aisle_test: $(AISLE_FILES)
	@echo "Building aisle_test"
	@$(CC) $(CFLAGS) $(AISLE_FILES) -o $@

clean:
	@echo "Cleaning/resetting build files"
	@$(RM) aisle_test store_test aisle_manager.o store_util.o
