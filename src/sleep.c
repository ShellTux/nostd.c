#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>

typedef int i32;
typedef long int i64;
typedef unsigned int u32;
typedef unsigned long int u64;
typedef int fd;
typedef unsigned char u8;

long syscall1(long number, long arg1);
long syscall2(long number, long arg1, long arg2);
long syscall3(long number, long arg1, long arg2, long arg3);

u64 nostd_write(const fd fd, const char *const buffer, const u64 length);
u64 parse_u64(const char *const string);
u64 string_length(const char *const string);
u64 u64_to_string(const u64 u64, char *string);
void nostd_sleep(const u64 seconds);
void sleeping(const u64 seconds);
void nostd_exit(const u8 code);

#define STDOUT_FILENO 1
#define WRITE(STRLIT) nostd_write(STDOUT_FILENO, STRLIT, sizeof(STRLIT) - 1)
#define ASSERT(COND, MESSAGE)                                                  \
  do {                                                                         \
    if (!(COND)) {                                                             \
      WRITE(MESSAGE "\n");                                                     \
      nostd_exit(1);                                                           \
    }                                                                          \
  } while (0)

#ifndef DEBUG
__attribute__((naked)) void _start() {
  __asm__ __volatile__("xor %ebp, %ebp\n"
                       "mov (%rsp), %rdi\n"
                       "lea 8(%rsp), %rsi\n"
                       "and $-16, %rsp\n"
                       "call main\n"
                       "mov %rax, %rdi\n"
                       "call nostd_exit\n");
}
#endif

int main(const int argc, const char *argv[]) {
  if (argc != 2) {
    const char *const program = argv[0];
    WRITE("Usage: ");
    nostd_write(STDOUT_FILENO, program, string_length(program));
    WRITE(" <seconds>\n");

    return 1;
  }

  const u64 seconds = parse_u64(argv[1]);

  sleeping(seconds);

  return 0;
}

u64 nostd_write(const fd fd, const char *const buffer, const u64 length) {
  return syscall3(SYS_write, fd, (long)buffer, length);
}

u64 parse_u64(const char *const string) {
  const u64 length = string_length(string);
  u64 result = 0;
  u64 power = 1;

  for (const char *d = string + length - 1; d >= string; d -= 1) {
    const u64 digit = *d - '0';
    ASSERT(/* 0 <= digit && */ digit <= 9, "Not a digit");

    result += digit * power;
    power *= 10;
  }

  return result;
}

u64 string_length(const char *const string) {
  const char *end = string;
  for (; *end != '\0'; end += 1) {
  }

  return (u64)(end - string);
}

u64 u64_to_string(const u64 number, char *string) {
  u64 rem = number;
  u64 length = 0;

  do {
    const u64 digit = rem % 10;

    string[length] = digit + '0';

    rem /= 10;
    length += 1;
  } while (rem > 0);

  // Invert string
  for (u64 i = 0; i < length / 2; i += 1) {
    const char c = string[i];
    string[i] = string[length - i - 1];
    string[length - i - 1] = c;
  }

  return length;
}

void nostd_sleep(const u64 seconds) {
  struct timespec {
    time_t tv_sec;  /* Seconds */
    time_t tv_nsec; /* Nanoseconds [0, 999'999'999] */
  };

  const struct timespec duration = {.tv_sec = seconds};
  syscall2(SYS_nanosleep, (long)(&duration), 0);
}

void nostd_exit(const u8 code) {
  syscall1(SYS_exit, code);
  for (;;) {
  }
}

void sleeping(const u64 seconds) {
  for (u64 rem = seconds; rem > 0; rem -= 1) {
    WRITE("\rSleeping: ");

    char secondsS[20] = {0};
    const u64 length = u64_to_string(rem, secondsS);
    nostd_write(STDOUT_FILENO, secondsS, length);

    nostd_sleep(1);
  }

  WRITE("\n");
}

long syscall1(long number, long arg1) {
  long result;

  __asm__ __volatile__("syscall"
                       : "=a"(result)
                       : "a"(number), "D"(arg1)
                       : "rcx", "r11", "memory");

  return result;
}

long syscall2(long number, long arg1, long arg2) {
  long result;

  __asm__ __volatile__("syscall"
                       : "=a"(result)
                       : "a"(number), "D"(arg1), "S"(arg2)
                       : "rcx", "r11", "memory");

  return result;
}

long syscall3(long number, long arg1, long arg2, long arg3) {
  long result;

  __asm__ __volatile__("syscall"
                       : "=a"(result)
                       : "a"(number), "D"(arg1), "S"(arg2), "d"(arg3)
                       : "rcx", "r11", "memory");

  return result;
}
