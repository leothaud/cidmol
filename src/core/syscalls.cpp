/* Copyright 2026 Dylan Leothaud
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
module;
#if defined(__linux__) and defined(__x86_64__)

export module core:syscalls;

import :builtins;
import :cstring;

namespace core {

export enum MmapProtection {
  PROT_READ = 0x1,
  PROT_WRITE = 0x2,
  PROT_EXEC = 0x4,
  PROT_SEM = 0x8,
  PROT_NONE = 0x0,
  PROT_GROWSDOWN = 0x1000000,
  PROT_GROWSUP = 0x2000000
};

export enum MmapFlags {
  MAP_SHARED = 0x01,
  MAP_PRIVATE = 0x02,
  MAP_SHARED_VALIDATE = 0x03,
  MAP_ANONYMOUS = 0x20
};

export enum FileProtection {
  O_RDONLY = 0,
  O_WRONLY = 1,
  O_RDWR = 2,
  O_CREAT = 100
};
export enum FseekDirection { SEEK_SET = 0, SEEK_CUR = 1, SEEK_END = 2 };

export enum CloneFlags {
  CLONE_VM = 0x00000100,
  CLONE_FS = 0x00000200,
  CLONE_FILES = 0x00000400,
  CLONE_SIGHAND = 0x00000800,
  CLONE_THREAD = 0x00010000,
  CLONE_SYSVSEM = 0x00040000,
  CLONE_SETTLS = 0x00080000,
  CLONE_PARENT_SETTID = 0x00100000,
  CLONE_CHILD_CLEARTID = 0x00200000,
};

export enum class FutexOp {
  WAIT = 0,
  WAKE = 1,
  FD = 2,
  REQUEUE = 3,
  CMP_REQUEUE = 4,
  WAKE_OP = 5,
  LOCK_PI = 6,
  UNLOCK_PI = 7,
  TRYLOCK_PI = 8,
  WAIT_BITSET = 9,
  WAKE_BITSET = 10,
  WAIT_REQUEUE_PI = 11,
  CMP_REQUEUE_PI = 12,
  LOCK_PI2 = 13,
};

export enum SyscallId {
  READ = 0,
  WRITE = 1,
  OPEN = 2,
  CLOSE = 3,
  STAT = 4,
  FSTAT = 5,
  LSTAT = 6,
  POLL = 7,
  LSEEK = 8,
  MMAP = 9,
  MPROTECT = 10,
  MUNMAP = 11,
  BRK = 12,
  RT_SIGACTION = 13,
  RT_SIGPROCMASK = 14,
  RT_SIGRETURN = 15,
  IOCTL = 16,
  PREAD64 = 17,
  PWRITE64 = 18,
  READV = 19,
  WRITEV = 20,
  ACCESS = 21,
  PIPE = 22,
  SELECT = 23,
  SCHED_YIELD = 24,
  MREMAP = 25,
  MSYNC = 26,
  MINCORE = 27,
  MADVISE = 28,
  SHMGET = 29,
  SHMAT = 30,
  SHMCTL = 31,
  DUP = 32,
  DUP2 = 33,
  PAUSE = 34,
  NANOSLEEP = 35,
  GETITIMER = 36,
  ALARM = 37,
  SETITIMER = 38,
  GETPID = 39,
  SENDFILE = 40,
  SOCKET = 41,
  CONNECT = 42,
  ACCEPT = 43,
  SENDTO = 44,
  RECVFROM = 45,
  SENDMSG = 46,
  RECVMSG = 47,
  SHUTDOWN = 48,
  BIND = 49,
  LISTEN = 50,
  GETSOCKNAME = 51,
  GETPEERNAME = 52,
  SOCKETPAIR = 53,
  SETSOCKOPT = 54,
  GETSOCKOPT = 55,
  CLONE = 56,
  FORK = 57,
  VFORK = 58,
  EXECVE = 59,
  EXIT = 60,
  WAIT4 = 61,
  KILL = 62,
  UNAME = 63,
  SEMGET = 64,
  SEMOP = 65,
  SEMCTL = 66,
  SHMDT = 67,
  MSGGET = 68,
  MSGSND = 69,
  MSGRCV = 70,
  MSGCTL = 71,
  FCNTL = 72,
  FLOCK = 73,
  FSYNC = 74,
  FDATASYNC = 75,
  TRUNCATE = 76,
  FTRUNCATE = 77,
  GETDENTS = 78,
  GETCWD = 79,
  CHDIR = 80,
  FCHDIR = 81,
  RENAME = 82,
  MKDIR = 83,
  RMDIR = 84,
  CREAT = 85,
  LINK = 86,
  UNLINK = 87,
  SYMLINK = 88,
  READLINK = 89,
  CHMOD = 90,
  FCHMOD = 91,
  CHOWN = 92,
  FCHOWN = 93,
  LCHOWN = 94,
  UMASK = 95,
  GETTIMEOFDAY = 96,
  GETRLIMIT = 97,
  GETRUSAGE = 98,
  SYSINFO = 99,
  TIMES = 100,
  PTRACE = 101,
  GETUID = 102,
  SYSLOG = 103,
  GETGID = 104,
  SETUID = 105,
  SETGID = 106,
  GETEUID = 107,
  GETEGID = 108,
  SETPGID = 109,
  GETPPID = 110,
  GETPGRP = 111,
  SETSID = 112,
  SETREUID = 113,
  SETREGID = 114,
  GETGROUPS = 115,
  SETGROUPS = 116,
  SETRESUID = 117,
  GETRESUID = 118,
  SETRESGID = 119,
  GETRESGID = 120,
  GETPGID = 121,
  SETFSUID = 122,
  SETFSGID = 123,
  GETSID = 124,
  CAPGET = 125,
  CAPSET = 126,
  RT_SIGPENDING = 127,
  RT_SIGTIMEDWAIT = 128,
  RT_SIGQUEUEINFO = 129,
  RT_SIGSUSPEND = 130,
  SIGALTSTACK = 131,
  UTIME = 132,
  MKNOD = 133,
  USELIB = 134,
  PERSONALITY = 135,
  USTAT = 136,
  STATFS = 137,
  FSTATFS = 138,
  SYSFS = 139,
  GETPRIORITY = 140,
  SETPRIORITY = 141,
  SCHED_SETPARAM = 142,
  SCHED_GETPARAM = 143,
  SCHED_SETSCHEDULER = 144,
  SCHED_GETSCHEDULER = 145,
  SCHED_GET_PRIORITY_MAX = 146,
  SCHED_GET_PRIORITY_MIN = 147,
  SCHED_RR_GET_INTERVAL = 148,
  MLOCK = 149,
  MUNLOCK = 150,
  MLOCKALL = 151,
  MUNLOCKALL = 152,
  VHANGUP = 153,
  MODIFY_LDT = 154,
  PIVOT_ROOT = 155,
  _SYSCTL = 156,
  PRCTL = 157,
  ARCH_PRCTL = 158,
  ADJTIMEX = 159,
  SETRLIMIT = 160,
  CHROOT = 161,
  SYNC = 162,
  ACCT = 163,
  SETTIMEOFDAY = 164,
  MOUNT = 165,
  UMOUNT2 = 166,
  SWAPON = 167,
  SWAPOFF = 168,
  REBOOT = 169,
  SETHOSTNAME = 170,
  SETDOMAINNAME = 171,
  IOPL = 172,
  IOPERM = 173,
  CREATE_MODULE = 174,
  INIT_MODULE = 175,
  DELETE_MODULE = 176,
  GET_KERNEL_SYMS = 177,
  QUERY_MODULE = 178,
  QUOTACTL = 179,
  NFSSERVCTL = 180,
  GETPMSG = 181,
  PUTPMSG = 182,
  AFS_SYSCALL = 183,
  TUXCALL = 184,
  SECURITY = 185,
  GETTID = 186,
  READAHEAD = 187,
  SETXATTR = 188,
  LSETXATTR = 189,
  FSETXATTR = 190,
  GETXATTR = 191,
  LGETXATTR = 192,
  FGETXATTR = 193,
  LISTXATTR = 194,
  LLISTXATTR = 195,
  FLISTXATTR = 196,
  REMOVEXATTR = 197,
  LREMOVEXATTR = 198,
  FREMOVEXATTR = 199,
  TKILL = 200,
  TIME = 201,
  FUTEX = 202,
  SCHED_SETAFFINITY = 203,
  SCHED_GETAFFINITY = 204,
  SET_THREAD_AREA = 205,
  IO_SETUP = 206,
  IO_DESTROY = 207,
  IO_GETEVENTS = 208,
  IO_SUBMIT = 209,
  IO_CANCEL = 210,
  GET_THREAD_AREA = 211,
  LOOKUP_DCOOKIE = 212,
  EPOLL_CREATE = 213,
  EPOLL_CTL_OLD = 214,
  EPOLL_WAIT_OLD = 215,
  REMAP_FILE_PAGES = 216,
  GETDENTS64 = 217,
  SET_TID_ADDRESS = 218,
  RESTART_SYSCALL = 219,
  SEMTIMEDOP = 220,
  FADVISE64 = 221,
  TIMER_CREATE = 222,
  TIMER_SETTIME = 223,
  TIMER_GETTIME = 224,
  TIMER_GETOVERRUN = 225,
  TIMER_DELETE = 226,
  CLOCK_SETTIME = 227,
  CLOCK_GETTIME = 228,
  CLOCK_GETRES = 229,
  CLOCK_NANOSLEEP = 230,
  EXIT_GROUP = 231,
  EPOLL_WAIT = 232,
  EPOLL_CTL = 233,
  TGKILL = 234,
  UTIMES = 235,
  VSERVER = 236,
  MBIND = 237,
  SET_MEMPOLICY = 238,
  GET_MEMPOLICY = 239,
  MQ_OPEN = 240,
  MQ_UNLINK = 241,
  MQ_TIMEDSEND = 242,
  MQ_TIMEDRECEIVE = 243,
  MQ_NOTIFY = 244,
  MQ_GETSETATTR = 245,
  KEXEC_LOAD = 246,
  WAITID = 247,
  ADD_KEY = 248,
  REQUEST_KEY = 249,
  KEYCTL = 250,
  IOPRIO_SET = 251,
  IOPRIO_GET = 252,
  INOTIFY_INIT = 253,
  INOTIFY_ADD_WATCH = 254,
  INOTIFY_RM_WATCH = 255,
  MIGRATE_PAGES = 256,
  OPENAT = 257,
  MKDIRAT = 258,
  MKNODAT = 259,
  FCHOWNAT = 260,
  FUTIMESAT = 261,
  NEWFSTATAT = 262,
  UNLINKAT = 263,
  RENAMEAT = 264,
  LINKAT = 265,
  SYMLINKAT = 266,
  READLINKAT = 267,
  FCHMODAT = 268,
  FACCESSAT = 269,
  PSELECT6 = 270,
  PPOLL = 271,
  UNSHARE = 272,
  SET_ROBUST_LIST = 273,
  GET_ROBUST_LIST = 274,
  SPLICE = 275,
  TEE = 276,
  SYNC_FILE_RANGE = 277,
  VMSPLICE = 278,
  MOVE_PAGES = 279,
  UTIMENSAT = 280,
  EPOLL_PWAIT = 281,
  SIGNALFD = 282,
  TIMERFD_CREATE = 283,
  EVENTFD = 284,
  FALLOCATE = 285,
  TIMERFD_SETTIME = 286,
  TIMERFD_GETTIME = 287,
  ACCEPT4 = 288,
  SIGNALFD4 = 289,
  EVENTFD2 = 290,
  EPOLL_CREATE1 = 291,
  DUP3 = 292,
  PIPE2 = 293,
  INOTIFY_INIT1 = 294,
  PREADV = 295,
  PWRITEV = 296,
  RT_TGSIGQUEUEINFO = 297,
  PERF_EVENT_OPEN = 298,
  RECVMMSG = 299,
  FANOTIFY_INIT = 300,
  FANOTIFY_MARK = 301,
  PRLIMIT64 = 302,
  NAME_TO_HANDLE_AT = 303,
  OPEN_BY_HANDLE_AT = 304,
  CLOCK_ADJTIME = 305,
  SYNCFS = 306,
  SENDMMSG = 307,
  SETNS = 308,
  GETCPU = 309,
  PROCESS_VM_READV = 310,
  PROCESS_VM_WRITEV = 311,
  KCMP = 312,
  FINIT_MODULE = 313,
  SCHED_SETATTR = 314,
  SCHED_GETATTR = 315,
  RENAMEAT2 = 316,
  SECCOMP = 317,
  GETRANDOM = 318,
  MEMFD_CREATE = 319,
  KEXEC_FILE_LOAD = 320,
  BPF = 321,
  EXECVEAT = 322,
  USERFAULTFD = 323,
  MEMBARRIER = 324,
  MLOCK2 = 325,
  COPY_FILE_RANGE = 326,
  PREADV2 = 327,
  PWRITEV2 = 328,
  PKEY_MPROTECT = 329,
  PKEY_ALLOC = 330,
  PKEY_FREE = 331,
  STATX = 332,
  IO_PGETEVENTS = 333,
  RSEQ = 334,
  PIDFD_SEND_SIGNAL = 424,
  IO_URING_SETUP = 425,
  IO_URING_ENTER = 426,
  IO_URING_REGISTER = 427,
  OPEN_TREE = 428,
  MOVE_MOUNT = 429,
  FSOPEN = 430,
  FSCONFIG = 431,
  FSMOUNT = 432,
  FSPICK = 433,
  PIDFD_OPEN = 434,
  CLONE3 = 435,
  CLOSE_RANGE = 436,
  OPENAT2 = 437,
  PIDFD_GETFD = 438,
  FACCESSAT2 = 439,
  PROCESS_MADVISE = 440,
  EPOLL_PWAIT2 = 441,
  MOUNT_SETATTR = 442,
  QUOTACTL_FD = 443,
  LANDLOCK_CREATE_RULESET = 444,
  LANDLOCK_ADD_RULE = 445,
  LANDLOCK_RESTRICT_SELF = 446,
  MEMFD_SECRET = 447,
  PROCESS_MRELEASE = 448,
  FUTEX_WAITV = 449,
  SET_MEMPOLICY_HOME_NODE = 450,
  CACHESTAT = 451,
  FCHMODAT2 = 452,
  MAP_SHADOW_STACK = 453,
  FUTEX_WAKE = 454,
  FUTEX_WAIT = 455,
  FUTEX_REQUEUE = 456,
  STATMOUNT = 457,
  LISTMOUNT = 458,
  LSM_GET_SELF_ATTR = 459,
  LSM_SET_SELF_ATTR = 460,
  LSM_LIST_MODULES = 461,
  MSEAL = 462
};

export const int stdin = 0, stdout = 1, stderr = 2;

#if __has_extension(gnu_asm)

export template <u64 code>
[[gnu::always_inline, maybe_unused,
  gnu::no_stack_protector]] u64 inline syscall() {
  u64 res;
  __asm__ volatile("syscall" : "=a"(res) : "a"(code) : "rcx", "r11", "memory");
  return res;
}

export template <u64 code>
[[gnu::always_inline, maybe_unused,
  gnu::no_stack_protector]] u64 inline syscall(u64 arg1) {
  u64 res;
  __asm__ volatile("syscall"
                   : "=a"(res)
                   : "a"(code), "D"(arg1)
                   : "rcx", "r11", "memory");
  return res;
}

export template <u64 code>
[[gnu::always_inline, maybe_unused,
  gnu::no_stack_protector]] u64 inline syscall(u64 arg1, u64 arg2) {
  u64 res;
  __asm__ volatile("syscall"
                   : "=a"(res)
                   : "a"(code), "D"(arg1), "S"(arg2)
                   : "rcx", "r11", "memory");
  return res;
}

export template <u64 code>
[[gnu::always_inline, maybe_unused,
  gnu::no_stack_protector]] u64 inline syscall(u64 arg1, u64 arg2, u64 arg3) {
  u64 res;
  __asm__ volatile("syscall"
                   : "=a"(res)
                   : "a"(code), "D"(arg1), "S"(arg2), "d"(arg3)
                   : "rcx", "r11", "memory");
  return res;
}

export template <u64 code>
[[gnu::always_inline, maybe_unused,
  gnu::no_stack_protector]] u64 inline syscall(u64 arg1, u64 arg2, u64 arg3,
                                               u64 arg4) {
  u64 res;
  register u64 r10 __asm__("r10") = arg4;
  __asm__ volatile("syscall"
                   : "=a"(res)
                   : "a"(code), "D"(arg1), "S"(arg2), "d"(arg3), "r"(r10)
                   : "rcx", "r11", "memory");
  return res;
}

export template <u64 code>
[[gnu::always_inline, maybe_unused,
  gnu::no_stack_protector]] u64 inline syscall(u64 arg1, u64 arg2, u64 arg3,
                                               u64 arg4, u64 arg5) {
  u64 res;
  register u64 r8 __asm__("r8") = arg5;
  register u64 r10 __asm__("r10") = arg4;
  __asm__ volatile("syscall"
                   : "=a"(res)
                   : "a"(code), "D"(arg1), "S"(arg2), "d"(arg3), "r"(r10),
                     "r"(r8)
                   : "rcx", "r11", "memory");
  return res;
}

export template <u64 code>
[[gnu::always_inline, maybe_unused,
  gnu::no_stack_protector]] u64 inline syscall(u64 arg1, u64 arg2, u64 arg3,
                                               u64 arg4, u64 arg5, u64 arg6) {
  u64 res;
  register u64 r8 __asm__("r8") = arg5;
  register u64 r9 __asm__("r9") = arg6;
  register u64 r10 __asm__("r10") = arg4;
  __asm__ volatile("syscall"
                   : "=a"(res)
                   : "a"(code), "D"(arg1), "S"(arg2), "d"(arg3), "r"(r10),
                     "r"(r8), "r"(r9)
                   : "rcx", "r11", "memory");
  return res;
}

#else
#error The syscalls needs __asm__ extensions for Linux x86_64.
#endif

export [[gnu::always_inline, gnu::no_stack_protector]] u64 archPrctl(u64 tls) {
  return syscall<ARCH_PRCTL>(0x1002, tls);
}

export [[gnu::noreturn, gnu::always_inline, gnu::no_stack_protector]] void
exit(u64 code) {
  syscall<EXIT>(code);
  unreachable();
}

export [[gnu::malloc]] void *mmap(void *ptr, u64 size, u64 protection,
                                  u64 flags, int fd, u64 offset) {
  return reinterpret_cast<void *>(syscall<MMAP>(
      reinterpret_cast<u64>(ptr), size, protection, flags, fd, offset));
}

export [[gnu::malloc]] void *mmap(u64 size, u64 protection, u64 flags) {
  return mmap(0, size, protection, flags, -1, 0);
}

export void munmap(void *ptr, u64 size) {
  syscall<MUNMAP>(reinterpret_cast<u64>(ptr), size);
}

export void write(int fd, const char *buffer, u32 size) {
  syscall<WRITE>(fd, reinterpret_cast<u64>(buffer), size);
}

export void write(int fd, const char *buffer) {
  write(fd, buffer, strlen(buffer));
}

export i64 read(int fd, char *buffer, u32 size) {
  return syscall<READ>(fd, reinterpret_cast<u64>(buffer), size);
}

export int open(const char *filename, u64 flags, u64 open) {
  return syscall<OPEN>(reinterpret_cast<u64>(filename), flags, open);
}

export int open(const char *filename) {
  return open(filename, O_RDWR | O_CREAT, 0666);
}

export void close(int fd) { syscall<CLOSE>(fd); }

export i64 seek(int fd, i64 offset, i64 whence) {
  return syscall<LSEEK>(fd, offset, whence);
}

export i64 getFileSize(int fd) {
  i64 currentOffset = seek(fd, 0, SEEK_CUR);
  i64 size = seek(fd, 0, SEEK_END);
  seek(fd, currentOffset, SEEK_SET);
  return size;
}

export [[gnu::malloc]] void *allocate(u64 size) {
  return reinterpret_cast<void *>(syscall<MMAP>(
      0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
}

export template <typename T> [[gnu::malloc]] void *allocate(int num = 1) {
  return allocate(sizeof(T) * num);
}

export void deallocate(void *ptr, u64 size) {
  syscall<MUNMAP>(reinterpret_cast<u64>(ptr), size);
}

export [[gnu::always_inline, gnu::no_stack_protector]] u64
wait4(u64 tid, u64 status, u64 options, u64 rusage) {
  return syscall<WAIT4>(tid, status, options, rusage);
}

export [[gnu::always_inline, gnu::no_stack_protector]] void msSleep(u64 time) {
  u64 duration[] = {time / 1000, time % 1000 * 1000000L};
  syscall<NANOSLEEP>(reinterpret_cast<u64>(duration),
                     reinterpret_cast<u64>(duration));
}

export [[gnu::always_inline, gnu::no_stack_protector]] void sleep(double time) {
  return msSleep(static_cast<u64>(1000.0 * time));
}

export [[gnu::always_inline, gnu::no_stack_protector]] void
futexWake(volatile int *futex) {
  syscall<FUTEX>(reinterpret_cast<u64>(futex), static_cast<u64>(FutexOp::WAKE),
                 1);
}

export [[gnu::always_inline, gnu::no_stack_protector]] void
futexWakeAll(volatile int *futex) {
  syscall<FUTEX>(reinterpret_cast<u64>(futex), static_cast<u64>(FutexOp::WAKE),
                 -1);
}

export [[gnu::always_inline, gnu::no_stack_protector]] void
futexWait(volatile int *futex, u64 val) {
  syscall<FUTEX>(reinterpret_cast<u64>(futex), static_cast<u64>(FutexOp::WAIT),
                 val, 0);
}

} // namespace core

#else
#error No alternative for syscalls.
#endif