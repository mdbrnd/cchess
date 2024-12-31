# Detect operating system
ifeq ($(OS),Windows_NT)
    PLATFORM = Windows
    CC = gcc
    EXT = .exe
    CFLAGS = -O2 -Wall -Wextra
    INCLUDES = -I include/
    LDFLAGS = -L lib/
    LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        PLATFORM = Linux
        CC = gcc
        EXT =
        CFLAGS = -O2 -Wall -Wextra
        INCLUDES = -I include/
        LDFLAGS = -L lib/
        LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    endif
    ifeq ($(UNAME_S),Darwin)
        PLATFORM = macOS
        CC = clang
        EXT =
        CFLAGS = -O2 -Wall -Wextra
        INCLUDES = -I include/
        LDFLAGS = -L lib/
        LIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    endif
endif

# Output executable name
TARGET = game$(EXT)

# Source files
SRC = main.c

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(SRC)
	@echo Building for $(PLATFORM)...
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(INCLUDES) $(LDFLAGS) $(LIBS)

# Clean target
clean:
	rm -f $(TARGET)

.PHONY: all clean