TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./$(TARGET) -f ./employee.db -n 
	./$(TARGET) -f ./employee.db -a "John Doe,101 Help Blvd.,40"
	./$(TARGET) -f ./employee.db -a "Jane Doe,123 Fake St.,32"
	./$(TARGET) -f ./employee.db -a "D.B Cooper,404 Nowhere Ln.,26"

default: $(TARGET)

check-leaks: $(TARGET)
	rm -f ./employee.db
	valgrind --leak-check=full ./$(TARGET) -f ./employee.db -n
	valgrind --leak-check=full ./$(TARGET) -f ./employee.db
	valgrind --leak-check=full ./$(TARGET) -f ./employee.db -a "D.B Cooper,404 Nowhere Ln.,26"
	valgrind --leak-check=full ./$(TARGET) -f ./employee.db -l

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o : src/%.c
	gcc -c $< -o $@ -Iinclude -g
