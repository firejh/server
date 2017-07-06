objects = \
    \
	xmlparse/xml_parse.o\
	xmlparse/xml_parse_doc.o\
	xmlparse/tinyxml.o\
	xmlparse/tinystr.o\
	xmlparse/tinyxmlerror.o\
	xmlparse/tinyxmlparser.o\
    string_kit.o\
	spinlock.o\
    exception.o\
    scope_guard.o\
    datetime.o\
    spinlock.o\
    timer.o\
	md5.o\
	signal_handler.o\

cc = g++

#cc_flags = -std=c++11 -fPIC -ggdb -Wall -Werror
cc_flags = -std=c++11 -ggdb -Wall -Werror $(optimization_flag)

target = libcomponent

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


