#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <stdio.h>

void show_backtrace (void) {
  unw_cursor_t cursor; unw_context_t uc;
  unw_word_t ip, sp;

  // take a snapshot of the CPU register states.
  unw_getcontext(&uc);

  // get a cursor based on the snapshot above.
  unw_init_local(&cursor, &uc);

  // setup up the stack or stack frame at a time.
  while (unw_step(&cursor) > 0) {
    unw_get_reg(&cursor, UNW_REG_IP, &ip); // get the instruction pointer (rip)
    unw_get_reg(&cursor, UNW_REG_SP, &sp); // get the stack pointer (rsp)
    printf ("ip = %lx, sp = %lx\n", (long) ip, (long) sp);
  }
}

int main(int argc, char** argv) {
  show_backtrace();
  return 0;
}
