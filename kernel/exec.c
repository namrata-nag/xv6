#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "elf.h"

int exec(char *path, char **argv)
{
  char *s, *last;
  int i, off;
  uint argc, sz, sp, ustack[3 + MAXARG + 1];
  struct elfhdr elf;
  struct inode *ip;
  struct proghdr ph;
  pde_t *pgdir, *oldpgdir;

  if ((ip = namei(path)) == 0)
    return -1;
  ilock(ip);
  pgdir = 0;

  // Check ELF header
  if (readi(ip, (char *)&elf, 0, sizeof(elf)) < sizeof(elf))
    goto bad;
  if (elf.magic != ELF_MAGIC)
    goto bad;

  if ((pgdir = setupkvm()) == 0)
    goto bad;

  // Load program into memory.
  sz = 0;
  for (i = 0, off = elf.phoff; i < elf.phnum; i++, off += sizeof(ph))
  {
    if (readi(ip, (char *)&ph, off, sizeof(ph)) != sizeof(ph))
      goto bad;
    if (ph.type != ELF_PROG_LOAD)
      continue;
    if (ph.memsz < ph.filesz)
      goto bad;
    if ((sz = allocuvm(pgdir, sz, ph.va + ph.memsz)) == 0)
      goto bad;
    if (loaduvm(pgdir, (char *)ph.va, ip, ph.offset, ph.filesz) < 0)
      goto bad;
  }
  iunlockput(ip);
  ip = 0;

  // Allocate a one-page stack at the next page boundary
  sz = PGROUNDUP(sz);
  if ((sz = allocuvm(pgdir, sz, sz + PGSIZE)) == 0)
    goto bad;

  // Push argument strings, prepare rest of stack in ustack.
  sp = sz;
  for (argc = 0; argv[argc]; argc++)
  {
    if (argc >= MAXARG)
      goto bad;
    sp -= strlen(argv[argc]) + 1;
    sp &= ~3;
    if (copyout(pgdir, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      goto bad;
    ustack[3 + argc] = sp;
  }
  ustack[3 + argc] = 0;

  ustack[0] = 0xffffffff; // fake return PC
  ustack[1] = argc;
  ustack[2] = sp - (argc + 1) * 4; // argv pointer

  sp -= (3 + argc + 1) * 4;
  if (copyout(pgdir, sp, ustack, (3 + argc + 1) * 4) < 0)
    goto bad;

  // Save program name for debugging.
  for (last = s = path; *s; s++)
    if (*s == '/')
      last = s + 1;
  safestrcpy(proc->name, last, sizeof(proc->name));

  // Commit to the user image.
  oldpgdir = proc->pgdir;
  proc->pgdir = pgdir;
  proc->sz = sz;
  proc->tf->eip = elf.entry; // main
  proc->tf->esp = sp;
  switchuvm(proc);
  freevm(oldpgdir);
  if (proc->pid == 1)
  {
    ptprint(proc->pgdir);
  }

  return 0;

bad:
  if (pgdir)
    freevm(pgdir);
  if (ip)
    iunlockput(ip);
  return -1;
}

pgdir 0xfe1000
..0: pde 0xfe0027 pa 0xfe0000
.. ..0: pte 0xfd4007 pa 0xfd4000
.. ..1: pte 0xfd3007 pa 0xfd3000
..1: pde 0xfdf007 pa 0xfdf000
..2: pde 0xfde007 pa 0xfde000
..3: pde 0xfdd027 pa 0xfdd000
..1016: pde 0xfdc007 pa 0xfdc000
..1017: pde 0xfdb007 pa 0xfdb000
..1018: pde 0xfda007 pa 0xfda000
..1019: pde 0xfd9027 pa 0xfd9000
..1020: pde 0xfd8007 pa 0xfd8000
..1021: pde 0xfd7007 pa 0xfd7000
..1022: pde 0xfd6007 pa 0xfd6000
..1023: pde 0xfd5007 pa 0xfd5000

pgdir 0xfe1000
..0: pde 0xfe0027 pa 0xfe0000
.. ..0: pte 0xfd4007 pa 0xfd4000
.. ..1: pte 0xfd3007 pa 0xfd3000
..1: pde 0xfdf007 pa 0xfdf000
..2: pde 0xfde007 pa 0xfde000
..3: pde 0xfdd027 pa 0xfdd000
..1016: pde 0xfdc007 pa 0xfdc000
..1017: pde 0xfdb007 pa 0xfdb000
..1018: pde 0xfda007 pa 0xfda000
..1019: pde 0xfd9007 pa 0xfd9000
..1020: pde 0xfd8007 pa 0xfd8000
..1021: pde 0xfd7007 pa 0xfd7000
..1022: pde 0xfd6007 pa 0xfd6000
..1023: pde 0xfd5007 pa 0xfd5000
