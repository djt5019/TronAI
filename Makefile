.SILENT:
DIRS	:= source
SOURCES := $(foreach DIR,$(DIRS),$(wildcard $(DIR)/*.cpp))
HEADERS := $(foreach DIR,$(DIRS),$(wildcard $(DIR)/*.h))
INCLUDE := $(SOURCES) $(HEADERS)

FLAGS:= -Wextra -Wall

test:
	g++ -g -D DEBUG $(FLAGS) -o out source/parser.cpp

final:
	g++ $(FLAGS) $(INCLUDE) -o out
	
clean:
	rm out
