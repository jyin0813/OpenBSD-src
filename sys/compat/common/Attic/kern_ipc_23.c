/*	$OpenBSD: kern_ipc_23.c,v 1.5 2004/07/15 11:25:59 millert Exp $	*/

/*
 * Copyright (c) 2004 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include <sys/mount.h>
#include <sys/syscallargs.h>

/*
 * Convert between new and old struct {msq,sem,shm}id_ds (both ways)
 */
#if defined(SYSVMSG) || defined(SYSVSEM) || defined(SYSVSHM)
#define cvt_ds(to, from, type, base) do {				\
	(to)->type##_perm.cuid = (from)->type##_perm.cuid;		\
	(to)->type##_perm.cgid = (from)->type##_perm.cgid;		\
	(to)->type##_perm.uid = (from)->type##_perm.uid;		\
	(to)->type##_perm.gid = (from)->type##_perm.gid;		\
	(to)->type##_perm.mode = (from)->type##_perm.mode & 0xffffU;	\
	(to)->type##_perm.seq = (from)->type##_perm.seq;		\
	(to)->type##_perm.key = (from)->type##_perm.key;		\
	bcopy((caddr_t)&(from)->base, (caddr_t)&(to)->base,		\
	    sizeof(*(to)) - ((caddr_t)&(to)->base - (caddr_t)to));	\
} while (0)
#endif /* SYSVMSG || SYSVSEM || SYSVSHM */

#ifdef SYSVMSG
/*
 * Copy a struct msqid_ds23 from userland and convert to struct msqid_ds
 */
static int
msqid_copyin(const void *uaddr, void *kaddr, size_t len)
{
	struct msqid_ds *msqbuf = kaddr;
	struct msqid_ds23 omsqbuf;
	int error;

	if (len != sizeof(struct msqid_ds))
		return (EFAULT);
	if ((error = copyin(uaddr, &omsqbuf, sizeof(omsqbuf))) == 0)
		cvt_ds(msqbuf, &omsqbuf, msg, msg_first);
	return (error);
}

/*
 * Convert a struct msqid_ds to struct msqid_ds23 and copy to userland
 */
static int
msqid_copyout(const void *kaddr, void *uaddr, size_t len)
{
	const struct msqid_ds *msqbuf = kaddr;
	struct msqid_ds23 omsqbuf;

	if (len != sizeof(struct msqid_ds))
		return (EFAULT);
	cvt_ds(&omsqbuf, msqbuf, msg, msg_first);
	return (copyout(&omsqbuf, uaddr, sizeof(omsqbuf)));
}

/*
 * OpenBSD 2.3 msgctl(2) with 16bit values in struct ipcperm.
 */
int
compat_23_sys_msgctl(struct proc *p, void *v, register_t *retval)
{
	struct compat_23_sys_msgctl_args /* {
		syscallarg(int) msqid;
		syscallarg(int) cmd;
		syscallarg(struct msqid_ds23 *) buf;
	} */ *uap = v;

	return (msgctl1(p, SCARG(uap, msqid), SCARG(uap, cmd),
	    (caddr_t)SCARG(uap, buf), msqid_copyin, msqid_copyout));
}
#endif /* SYSVMSG */

#ifdef SYSVSEM
/*
 * Copy a struct semid_ds23 from userland and convert to struct semid_ds
 */
static int
semid_copyin(const void *uaddr, void *kaddr, size_t len)
{
	struct semid_ds *sembuf = kaddr;
	struct semid_ds23 osembuf;
	int error;

	if (len != sizeof(struct semid_ds))
		return (EFAULT);
	if ((error = copyin(uaddr, &osembuf, sizeof(osembuf))) == 0)
		cvt_ds(sembuf, &osembuf, sem, sem_base);
	return (error);
}

/*
 * Convert a struct semid_ds to struct semid_ds23 and copy to userland
 */
static int
semid_copyout(const void *kaddr, void *uaddr, size_t len)
{
	const struct semid_ds *sembuf = kaddr;
	struct semid_ds23 osembuf;

	if (len != sizeof(struct semid_ds))
		return (EFAULT);
	cvt_ds(&osembuf, sembuf, sem, sem_base);
	return (copyout(&osembuf, uaddr, sizeof(osembuf)));
}

/*
 * OpenBSD 2.3 semctl(2) with 16bit values in struct ipcperm.
 */
int
compat_23_sys___semctl(struct proc *p, void *v, register_t *retval)
{
	struct compat_23_sys___semctl_args /* {
		syscallarg(int) semid;
		syscallarg(int) semnum;
		syscallarg(int) cmd;
		syscallarg(union semun *) arg;
	} */ *uap = v;
	union semun arg;
	int error = 0, cmd = SCARG(uap, cmd);

	switch (cmd) {
	case IPC_SET:
	case IPC_STAT:
	case GETALL:
	case SETVAL:
	case SETALL:
		error = copyin(SCARG(uap, arg), &arg, sizeof(arg));
		break;
	}
	if (error == 0) {
		error = semctl1(p, SCARG(uap, semid), SCARG(uap, semnum),
		    cmd, &arg, retval, semid_copyin, semid_copyout);
	}
	return (error);
}
#endif /* SYSVSEM */

#ifdef SYSVSHM
/*
 * Copy a struct shmid_ds23 from userland and convert to struct shmid_ds
 */
static int
shmid_copyin(const void *uaddr, void *kaddr, size_t len)
{
	struct shmid_ds *shmbuf = kaddr;
	struct shmid_ds23 oshmbuf;
	int error;

	if (len != sizeof(struct shmid_ds))
		return (EFAULT);
	if ((error = copyin(uaddr, &oshmbuf, sizeof(oshmbuf))) == 0)
		cvt_ds(shmbuf, &oshmbuf, shm, shm_segsz);
	return (error);
}

/*
 * Convert a struct shmid_ds to struct shmid_ds23 and copy to userland
 */
static int
shmid_copyout(const void *kaddr, void *uaddr, size_t len)
{
	const struct shmid_ds *shmbuf = kaddr;
	struct shmid_ds23 oshmbuf;

	if (len != sizeof(struct shmid_ds))
		return (EFAULT);
	cvt_ds(&oshmbuf, shmbuf, shm, shm_segsz);
	return (copyout(&oshmbuf, uaddr, sizeof(oshmbuf)));
}

/*
 * OpenBSD 2.3 shmctl(2) with 16bit values in struct ipcperm.
 */
int
compat_23_sys_shmctl(struct proc *p, void *v, register_t *retval)
{
	struct compat_23_sys_shmctl_args /* {
		syscallarg(int) shmid;
		syscallarg(int) cmd;
		syscallarg(struct shmid_ds23 *) buf;
	} */ *uap = v;

	return (shmctl1(p, SCARG(uap, shmid), SCARG(uap, cmd),
	    (caddr_t)SCARG(uap, buf), shmid_copyin, shmid_copyout));
}
#endif /* SYSVSHM */
