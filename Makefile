
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

PYTHON ?= python

RENDER_JINJA_SCRIPT ?= $(DEPS_DIR)/render-jinja/render_jinja.py

RENDER_JINJA ?= $(PYTHON) $(RENDER_JINJA_SCRIPT)

types := bool ord char schar uchar short ushort int uint long ulong \
         llong ullong int8 uint8 int16 uint16 int32 uint32 \
         intmax uintmax ptrdiff wchar size ptr constptr str mutstr

int8_type     := int8_t
uint8_type    := uint8_t
int16_type    := int16_t
uint16_type   := uint16_t
int32_type    := int32_t
uint32_type   := uint32_t
intmax_type   := intmax_t
uintmax_type  := uintmax_t
ptrdiff_type  := ptrdiff_t
wchar_type    := wchar_t
size_type     := size_t
ptr_type      := void *
constptr_type := void const *
str_type      := char const *
mutstr_type   := char *

common_typeclasses := EQ

prefix := array-

name_to_header_path = $(prefix)$(1).h
name_to_source_path = $(prefix)$(1).c

gen_headers := $(foreach t,$(types),$(call name_to_header_path,$(t)))
gen_sources := $(foreach t,$(types),$(call name_to_source_path,$(t)))

sources := $(wildcard *.c) $(gen_sources)
objects := $(sources:.c=.o)
mkdeps  := $(sources:.c=.dep.mk)



##############################
### BUILDING
##############################

.PHONY: all
all: $(objects)

.PHONY: fast
fast: CPPFLAGS += -DNDEBUG -DNO_ASSERT -DNO_REQUIRE -DNO_DEBUG
fast: CFLAGS = $(cflags_std) -O3 $(cflags_warnings)
fast: all


uc = $(shell echo $(1) | tr [:lower:] [:upper:])

$(gen_headers) $(gen_sources): $(RENDER_JINJA_SCRIPT)

$(gen_headers): $(prefix)%.h: header.h.jinja
	$(eval up := $(call uc,$*))
	$(RENDER_JINJA) $< "include_guard=LIBARRAY_ARRAY_$(up)_H" "sys_headers=libbase/$*.h" "rel_headers=" "type=$(or $($(*)_type),$*)" "macroname=$(up)" "typename=$*" "funcname=$*" "typeclasses=$(common_typeclasses) $($(*)_typeclasses)" -o $@

$(gen_sources): %.c: %.h
$(gen_sources): $(prefix)%.c: source.c.jinja
	$(eval up := $(call uc,$*))
	$(RENDER_JINJA) $< "header=$(call name_to_header_path,$*)" "sys_headers=libbase/$*.h" "rel_headers=" "type=$(or $($(*)_type),$*)" "macroname=$(up)" "typename=$*" "funcname=$*" "typeclasses=$(common_typeclasses) $($(*)_typeclasses)" -o $@


.PHONY: clean
clean:
	rm -rf $(gen_sources) $(gen_headers) $(objects) $(mkdeps)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MF "$(@:.o=.dep.mk)" -c $< -o $@


-include $(mkdeps)


