/*
**
**
*/


/*
**--START AUTOVER block--
*/

#define __ASL_VERSION(major,minor,build) \
	const unsigned char	SYS_major = major; \
	const unsigned char	SYS_minor = minor; \
	const unsigned short	SYS_build = build; \

/*
**--END AUTOVER block--
*/


/*
** Specify the current version number here. The actual numbers must be specified
** so don't hide them behind another #define.
**
** Major, Minor, Build (	don't use leading zero for minor number - SETVER does
**					that sort of thing for you, and  C will reject 08
** 					as it is an illegal octal number).
*/

/*lint -esym(714,SYS_version) Unref */
__ASL_VERSION( 0 , 9 , 47 )
/**AUTOVER** [version <M>.<mm>.<bbbb>] */
const char	SYS_version[] = "@(#)version 0.09.0047";


