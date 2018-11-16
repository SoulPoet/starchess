CC = g++
OBJECTS = starchess23.o
PROJECT = starchess

$(PROJECT) : $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

%.o : %.cpp
	$(CC) -c $< -o $@

clean :
	rm -rf $(OBJECTS) $(PROJECT)