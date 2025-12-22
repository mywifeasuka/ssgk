/*
 * contains the implementation of all syscalls.
 */

#include <stdint.h>
#include <errno.h>

#include "util/types.h"
#include "syscall.h"
#include "string.h"
#include "process.h"
#include "util/functions.h"

#include "spike_interface/spike_utils.h"

//
// implement the SYS_user_print syscall
//
ssize_t sys_user_print(const char* buf, size_t n) {
  sprint(buf);
  return 0;
}

//
// implement the SYS_user_exit syscall
//
ssize_t sys_user_exit(uint64 code) {
  sprint("User exit with code:%d.\n", code);
  // in lab1, PKE considers only one app (one process). 
  // therefore, shutdown the system when the app calls exit()
  shutdown(code);
  return 0;
}

//
// [新增] implement the SYS_user_backtrace syscall
// added @lab1_challenge1
//
ssize_t sys_user_backtrace(int64_t fp) {
  sprint("back trace the user app in the following:\n");

  // RISC-V 栈帧结构回溯逻辑：
  // fp Points to the stack bottom of current function
  // fp - 8  : Return Address (ra)
  // fp - 16 : Previous Frame Pointer (old fp)
  while (fp != 0) {
      uint64 ra = *(uint64*)(fp - 8);
      uint64 prev_fp = *(uint64*)(fp - 16);

      print_symbol(ra); // PKE 提供的工具函数，打印函数名
      sprint("\n");

      fp = prev_fp; // 向上移动到调用者的栈帧
  }
  return 0;
}

//
// [a0]: the syscall number; [a1] ... [a7]: arguments to the syscalls.
// returns the code of success, (e.g., 0 means success, fail for otherwise)
//
long do_syscall(long a0, long a1, long a2, long a3, long a4, long a5, long a6, long a7) {
  // 根据你的 strap.c: do_syscall(tf->a7, tf->a0, tf->a1, ...)
  // a0 对应 tf->a7 (系统调用号)
  // a1 对应 tf->a0 (第一个参数)
  // a2 对应 tf->a1 (第二个参数)
  
  switch (a0) {
    case SYS_user_print:
      // print(buf, len)
      // buf 在 tf->a0 (即这里的 a1)
      // len 在 tf->a1 (即这里的 a2)
      return sys_user_print((const char*)a1, a2);

    case SYS_user_exit:
      // exit(code)
      // code 在 tf->a0 (即这里的 a1)
      return sys_user_exit(a1);

    case SYS_user_backtrace:
      // backtrace(fp)
      // fp 在 tf->a0 (即这里的 a1)
      return sys_user_backtrace(a1);

    default:
      panic("Unknown syscall %ld \n", a0);
  }
}