start_RtDebugMode()
{
	mkdir /dev/pts
	mknod -m 666 /dev/ptmx c 5 2
	mknod -m 666 /dev/pts/0 c 136 0
	mknod -m 666 /dev/pts/1 c 136 1

	killall utelnetd
	killall telnetenable

	/usr/sbin/telnetenable
}
