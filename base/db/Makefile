objects = \
    \
	db_connector.o\
	db_select_result.o\
	db_commond.o\

cc = g++

#cc_flags = -std=c++11 -fPIC -ggdb -Wall -Werror
cc_flags = -std=c++11 -ggdb -Wall -Werror $(optimization_flag)\
-I../

target = libdb

.PHONY:
all: $(target).a 

$(target).a : $(objects)
	ar rs $(target).a $(objects)

$(target).so : $(objects)
	$(cc) -shared $(objects) -o $(target).so

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
	rm -f $(objects) $(objects:.o=.d) *.d *.d.* */*.d
	rm -f $(target).a

distclean: clean


