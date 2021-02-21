SUBDIRS = cli core
				
all: subdirs
	gcc -o super-fs core/core.o cli/cli.o 

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	        $(MAKE) -C $@

clean:
	find . -type f -name '*.out' -delete
	rm super-fs

	for dir in $(SUBDIRS); do \
	$(MAKE) clean -C $$dir; \
	done
