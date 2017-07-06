objects = \
    \
    process_id.o\
    process_config.o\
    process_thread.o\
    tcp_client.o\
    tcp_server.o\
    tcp_packet.o\
	connection_checker.o\
    tcp_connection_manager.o\
	private_connection_checker.o\
    process_timer.o\
	http_packet.o\
	http_connection_manager.o\
    process.o\
	db_server.o\
	shell_arg_parser.o\



CC = g++
cc = g++

cc_flags = -std=c++11 -ggdb -Wall -Werror $(optimization_flag) \
		   -I../
ln_flags = -lpthread 

target = libprocess.a

.PHONY:
all: $(target)

$(target) : $(objects)
#	@set -e -x; \
#	echo $(ln_flags)
#	g++ $(objects) $(ln_flags) -o $(target) 
	ar rs $(target) $(objects)

%.o: %.cpp
	$(cc) $(cc_flags) -c $< -o $@

%.d: %.cpp
#此行用于调试	@set -e -x; \ #此行用于调试
	@rm -f $@; \
	$(cc) $(cc_flags) -MM $< > $@.$$$$;  \
	sed -r 's,^(.*)\.o:,$@ $*\.o:,' $@.$$$$ > $@;  \
	rm -f $@.$$$$

-include $(objects:.o=.d)

.PHONY: clean distclean
clean:
	rm -f $(objects) $(objects:.o=.d) *.d *.d.* 
	rm -f $(target)

distclean: clean
