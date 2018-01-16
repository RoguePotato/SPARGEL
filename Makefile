CC := g++

SRCDIR := src
BUILDDIR := build
TARGET := bin/spargel

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
DEPS := $(OBJECTS:.o=.d)

INC := -I ./include
CPPFLAGS := $(INC) -std=c++11 -MMD -MP -pthread -O2
LIBS := -lpthread

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $(TARGET) $(LIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CPPFLAGS) $(INC) -c -o $@ $<

clean:
	$(RM) $(TARGET) $(OBJECTS) $(DEPS)

.PHONY: clean
-include $(DEPS)
