
RELEASE= 0.4.0

install = /usr/bin/install


prefix = /usr/local
exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin
libdir = ${exec_prefix}/lib

all:
	@cd category_test && make 
	@cd random_set && make 
	@cd refinement && make

clean:
	@cd category_test && make clean
	@cd random_set && make clean
	@cd refinement && make clean

dist-clean:
	@cd category_test && make dist-clean
	@cd random_set && make dist-clean
	@cd refinement && make dist-clean
	
install:
	@cd category_test && make install
	@cd random_set && make install
	@cd refinement && make install
