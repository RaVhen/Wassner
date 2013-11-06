LIBS          := -lcsfml-graphics	
SOURCES       := $(CURDIR)
EXEC          := fenetre
CFILES        := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))

all:
	gcc -Wall -g -L/usr/lib/x86_64-linux-gnu/-lm $(CFILES) $(LIBS) -o $(EXEC)
clean:
	rm $(EXEC)
