LIBS          := -lcsfml-graphics	
SOURCES       := $(CURDIR)
EXEC          := fenetre
CFILES        := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))

all:
	gcc -Wall -g -lm $(CFILES) $(LIBS) -o $(EXEC)
clean:
	rm $(EXEC)
