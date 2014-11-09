# Converts README.md to pdf and html
all:
	@echo "Try make [lib | examples | python | kodiak]"

lib:
	cd src && $(MAKE)

examples:
	cd examples && $(MAKE)

python:
	cd python && $(MAKE) python

kodiak:
	cd python && $(MAKE) kodiak

doc:
	pandoc -o README.pdf README.md
	pandoc -o README.html README.md

.PHONY : lib examples python kodiak doc
