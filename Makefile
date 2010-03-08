.SILENT:
DIRS	:= source
SOURCES := $(foreach DIR,$(DIRS),$(wildcard $(DIR)/*.cpp))
HEADERS := $(foreach DIR,$(DIRS),$(wildcard $(DIR)/*.h))
INCLUDE := $(SOURCES) $(HEADERS)

FLAGS:= -Wextra -Wall

all:
	g++ $(FLAGS) $(INCLUDE) -o out
	

