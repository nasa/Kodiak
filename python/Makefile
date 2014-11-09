export CFLAGS=-I/usr/include/ -I/usr/local/include/ -I/opt/local/include/ -L/usr/lib -L/usr/local/lib -L/opt/local/lib -I$(BOOST_ROOT)/include

arch=$(shell uname -s)

ifeq ($(arch),Darwin)
	myarch = macos
else
ifeq ($(arch),Linux)
	myarch = linux
else
	myarch = $(arch)
endif
endif

# Compiles and link the interface and creates seld contained executable
all: 
	@echo "Try make [python | kodiak]"

python: clean
	@python setup.py build_ext --inplace
	@python setup.py clean
	@echo "**"
	@echo "** Kodiak: Python Interface built in direcotry python"
	@echo "**"

kodiak: 
	pyinstaller kodiak.py --hidden-import=numbers -y --clean --onefile --name=kodiak --distpath=../bin
	@echo "**"
	@echo "** Kodiak: Binary built in directory bin"
	@echo "**"


# Removes compile and execution byproducts
clean:
	rm -rf build
	python setup.py clean --all
	rm -f *parsetab.py *.dat *.so *.pyc interface.cpp *.o *.spec
	@echo "Clean complete"
	@echo

.PHONY : python all python kodiak
