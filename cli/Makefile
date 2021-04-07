OBJ_FILES = interactive


.SILENT: clean-temp

all: $(OBJ_FILES)
	        ld -relocatable $(OBJ_FILES) -o cli.o
					make clean-temp

$(OBJ_FILES):
	        gcc -c src/$@.c -o $@

clean:
	        rm *.o

clean-temp:
	        for file in $(OBJ_FILES); do \
						  rm $$file;      \
					done;
