make clean
make -DOUTPUT_DIR=..\SOURCE\HIDACDRV c167.lno
copy hidac.lno ..\SOURCE\HIDACDRV
copy *.h ..\SOURCE\HIDACDRV /y

