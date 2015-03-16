
##############################
### VARIABLES
##############################

DEPS_DIR ?= ./deps

CPPFLAGS += -I$(DEPS_DIR)

cflags_std := -std=c11
cflags_warnings := -Wall -Wextra -pedantic \
                   -Wcomments -Wformat=2 -Wlogical-op -Wmissing-include-dirs \
                   -Wnested-externs -Wold-style-definition -Wredundant-decls \
                   -Wshadow -Wstrict-prototypes -Wunused-macros -Wvla \
                   -Wwrite-strings \
                   -Wno-override-init -Wno-unused-parameter

CFLAGS ?= $(cflags_std) -g $(cflags_warnings)

TPLRENDER ?= $(DEPS_DIR)/tplrender/tplrender


name_from_path = $(subst -,_,$(basename $(notdir $1)))


test_libarray_types := short uintmax ptrm-int
test_libbase_types := $(test_libarray_types) size
test_libmaybe_type := size

short_type      := short
short_options   := --typeclasses BOUNDED EQ ORD ENUM NUM \
                   --extra num_type=signed min_bound=SHRT_MIN max_bound=SHRT_MAX

uintmax_type    := uintmax_t
uintmax_options := --typeclasses BOUNDED EQ ORD ENUM NUM \
                   --extra num_type=unsigned

ptrm_int_type    := int *
ptrm_int_options := --typeclasses EQ ORD

size_type    := size_t
size_options := --typeclasses BOUNDED EQ ORD ENUM NUM \
                --extra num_type=unsigned

test_libbase_sources := $(foreach t,$(test_libbase_types),$(DEPS_DIR)/libbase/$t.c)
test_libbase_headers := $(test_libbase_sources:.c=.h)
test_libbase_objects := $(test_libbase_sources:.c=.o)

test_libmaybe_defs := $(foreach t,$(test_libmaybe_defs),$(DEPS_DIR)/libmaybe/def/maybe-size.h)

test_libarray_sources := $(foreach t,$(test_libarray_types),array-$t.c)
test_libarray_headers := $(test_libarray_sources:.c=.h)
test_libarray_defs    := $(addprefix def/,$(test_libarray_headers))
test_libarray_objects := $(test_libarray_sources:.c=.o)

test_gen_objects := $(test_libbase_objects) \
                    $(test_libarray_objects)

test_gen := $(test_libbase_sources) \
            $(test_libbase_headers) \
            $(test_libmaybe_defs) \
            $(test_libarray_sources) \
            $(test_libarray_headers) \
            $(test_libarray_defs) \
            $(test_libarray_objects) \
            $(test_gen_objects)

test_binaries := tests/test

mkdeps := $(test_gen_objects:.o=.dep.mk)



##############################
### BUILDING
##############################

.PHONY: all
all: tests

.PHONY: fast
fast: CPPFLAGS += -DNDEBUG
fast: CFLAGS = $(cflags_std) -O3 $(cflags_warnings)
fast: all

.PHONY: tests
tests: $(test_binaries)

.PHONY: test
test: tests
	@./tests/test

.PHONY: clean
clean:
	rm -rf $(test_gen) $(test_binaries) $(mkdeps)


%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MF "$(@:.o=.dep.mk)" -c $< -o $@


tests/test: $(test_gen_objects)

$(test_libbase_headers): %.h: $(DEPS_DIR)/libbase/header.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(test_libbase_sources): %.c: $(DEPS_DIR)/libbase/source.c.jinja %.h
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(test_libmaybe_defs): $(DEPS_DIR)/libmaybe/def/maybe-%.h: $(DEPS_DIR)/libmaybe/def.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(test_libarray_defs): def/array-%.h: def.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(test_libarray_headers): array-%.h: header.h.jinja def/array-%.h
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(test_libarray_sources): array-%.c: source.c.jinja array-%.h $(DEPS_DIR)/libbase/%.h $(DEPS_DIR)/libmaybe/def/maybe-size.h
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) --sys-headers "libbase/$*.h" -o $@


-include $(mkdeps)


