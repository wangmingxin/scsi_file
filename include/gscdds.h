/*
 * $Id: s.gscdds.h 1.1 96/12/29 19:49:33-00:00 mjacob@bird.feral.com $
 * Copyright (c) 1996, 1997 by Matthew Jacob
 *
 *	This software is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Library General Public
 *	License as published by the Free Software Foundation; version 2.
 *
 *	This software is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *	Library General Public License for more details.
 *
 *	You should have received a copy of the GNU Library General Public
 *	License along with this software; if not, write to the Free
 *	Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *	The author may be reached via electronic communications at
 *
 *		mjacob@feral.com
 *
 *	or, via United States Postal Address
 *
 *		Matthew Jacob
 *		1831 Castro Street
 *		San Francisco, CA, 94131
 */
#ifndef	_gscdds_h
#define	_gscdds_h

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * Structure used to convey a scsi command to the driver
 */
typedef struct {
    char *	cdb;				/* SCSI command block */
    int 	cdblen;				/* length of SCSI command block */
    char *	data_buf;			/* pointer to data area */
    int		datalen;			/* length of data area. Zero implies none */
    char *	sense_buf;			/* pointer to SENSE DATA area */
    int		senselen;			/* length of sense area. Zero implies none */
    char *	statusp;			/* pointer to SCSI status byte */
    int		rw;					/* direction of data transfer- 1 means read */
    int		timeval;			/* secs to complete cmd- 0 means infinite */
} scmd_t;

#ifdef __64BIT__
#define	GSC_CMD		(('G' << 8) | 2)
#else  /* __64BIT__ */
#define	GSC_CMD		(('G' << 8) | 0)
#endif /* __64BIT__ */
#define	GSC_SETDBG	(('G' << 8) | 1)

#ifdef	__cplusplus
}
#endif
#endif	/* ! _gscdds_h */
/*
 * mode: c
 * Local variables:
 * c-indent-level: 4
 * c-brace-imaginary-offset: 0
 * c-brace-offset: -4
 * c-argdecl-indent: 4
 * c-label-offset: -4
 * c-continued-statement-offset: 4
 * c-continued-brace-offset: 0
 * End:
 */
