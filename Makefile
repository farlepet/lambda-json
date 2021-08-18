MAINDIR    = .
SRC        = $(MAINDIR)/src
INC        = $(MAINDIR)/inc
TESTDIR    = $(MAINDIR)/tests
BUILDDIR   = $(MAINDIR)/build

SRCS       = $(wildcard $(SRC)/*.c)
TESTSRCS   = $(wildcard $(TESTDIR)/*.c)

OBJS       = $(patsubst %.c,$(BUILDDIR)/%.o,$(SRCS))
DEPS       = $(patsubst %.c,$(BUILDDIR)/%.d,$(SRCS))

TESTS      = $(patsubst %.c,$(BUILDDIR)/%,$(TESTSRCS))
DEPS      += $(patsubst %.c,$(BUILDDIR)/tests/%.d,$(TESTSRCS))

CFLAGS    += -Wall -Wextra -Werror -I $(INC)
ifeq ($(CC), clang)
# TODO: Take the time to go through all these -Wno- commands to fix easy-to-fix errors
CFLAGS    += -Weverything \
             -Wno-padded
endif

ifeq ($(DEBUG), 1)
CFLAGS    += -DLJSON_DEBUG
endif

OUT        = libljson.a

.PHONY: all clean link

all: link

tests: $(TESTS)

link:   $(OBJS)
	@echo -e "\033[33m  \033[1mCombining Objects\033[0m"
	@ar rvs --target=elf32-i386 $(OUT) $(OBJS) &> /dev/null

$(BUILDDIR)/%: %.c $(OUT)
	@echo -e "\033[32m  \033[1mCC\033[21m    \033[34m$<\033[0m"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ $< $(OUT)

# gcc:
$(BUILDDIR)/%.o: %.c
	@echo -e "\033[32m  \033[1mCC\033[21m    \033[34m$<\033[0m"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $<


clean:
	@rm -f $(OBJS) $(TESTS) $(OUT)

-include $(DEPS)
