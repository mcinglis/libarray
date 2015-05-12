
##############################
### VARIABLES
##############################

DEPS_DIR ?= ./deps

LIBBASE  := $(DEPS_DIR)/libbase
LIBMAYBE := $(DEPS_DIR)/libmaybe

CPPFLAGS += -I$(DEPS_DIR)

CFLAGS ?= -std=c11 -g \
          -Wall -Wextra -pedantic \
          -Wcomments -Wformat=2 -Wlogical-op -Wmissing-include-dirs \
          -Wnested-externs -Wold-style-definition -Wredundant-decls \
          -Wshadow -Wstrict-prototypes -Wunused-macros -Wvla -Wwrite-strings \
          -Wno-override-init -Wno-type-limits -Wno-unused-parameter

TPLRENDER ?= $(DEPS_DIR)/tplrender/tplrender


libarray_types := short uintmax ptrm-int
libbase_types  := $(libarray_types) size
libmaybe_types := $(libarray_types) size

short_type       := short
short_options    := --typeclasses BOUNDED EQ ORD ENUM NUM \
                    --extra num_type=signed min_bound=SHRT_MIN max_bound=SHRT_MAX

uintmax_type     := uintmax_t
uintmax_options  := --typeclasses BOUNDED EQ ORD ENUM NUM \
                    --extra num_type=unsigned

ptrm_int_type    := int *
ptrm_int_options := --typeclasses EQ ORD

size_type        := size_t
size_options     := --typeclasses BOUNDED EQ ORD ENUM NUM \
                    --extra num_type=unsigned

libbase_sources := $(foreach t,$(libbase_types),$(LIBBASE)/$t.c)
libbase_headers := $(libbase_sources:.c=.h)
libbase_objects := $(libbase_sources:.c=.o)

libmaybe_sources := $(foreach t,$(libmaybe_types),$(LIBMAYBE)/maybe_$t.c)
libmaybe_headers := $(libmaybe_sources:.c=.h)
libmaybe_defs    := $(foreach t,$(libmaybe_types),$(LIBMAYBE)/def/maybe_$t.h)
libmaybe_objects := $(libmaybe_sources:.c=.o)

libarray_sources := $(foreach t,$(libarray_types),array_$t.c)
libarray_headers := $(libarray_sources:.c=.h)
libarray_defs    := $(addprefix def/,$(libarray_headers))
libarray_objects := $(libarray_sources:.c=.o)

gen_objects := \
    $(libbase_objects) \
    $(libmaybe_objects) \
    $(libarray_objects)

gen := \
    $(libbase_sources) \
    $(libbase_headers) \
    $(libmaybe_defs) \
    $(libmaybe_sources) \
    $(libmaybe_headers) \
    $(libarray_defs) \
    $(libarray_sources) \
    $(libarray_headers) \
    $(gen_objects)

test_binaries := $(basename $(wildcard tests/*.c))

mkdeps := $(gen_objects:.o=.dep.mk)



##############################
### BUILDING
##############################

.PHONY: all
all: tests

.PHONY: tests
tests: $(test_binaries)

.PHONY: test
test: tests
	./tests/test

.PHONY: clean
clean:
	rm -rf $(gen) $(test_binaries) $(mkdeps)


%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MF "$(@:.o=.dep.mk)" -c $< -o $@


tests/test: $(gen_objects)

name_from_path = $(subst -,_,$1)

$(libbase_headers): $(LIBBASE)/%.h: $(LIBBASE)/header.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(libbase_sources): $(LIBBASE)/%.c: $(LIBBASE)/source.c.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(libbase_objects): $(LIBBASE)/%.o: $(LIBBASE)/%.h

$(libmaybe_defs): $(LIBMAYBE)/def/maybe_%.h: $(LIBMAYBE)/def.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(libmaybe_headers): $(LIBMAYBE)/maybe_%.h: $(LIBMAYBE)/header.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(libmaybe_sources): $(LIBMAYBE)/maybe_%.c: $(LIBMAYBE)/source.c.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) --sys-headers "libbase/$*.h" -o $@

$(libmaybe_objects): $(LIBMAYBE)/maybe_%.o: \
    $(LIBMAYBE)/def/maybe_%.h \
    $(LIBMAYBE)/maybe_%.h \
    $(LIBBASE)/%.h

$(libarray_defs): def/array_%.h: def.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(libarray_headers): array_%.h: header.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(libarray_sources): array_%.c: source.c.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) --sys-headers "libbase/$*.h" -o $@

$(libarray_objects): array_%.o: \
    array_%.h \
    def/array_%.h \
    $(LIBBASE)/%.h \
    $(LIBBASE)/size.h \
    $(LIBMAYBE)/def/maybe_size.h \
    $(LIBMAYBE)/def/maybe_%.h \
    $(LIBMAYBE)/maybe_%.h


-include $(mkdeps)


