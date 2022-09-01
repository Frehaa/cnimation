CC = cl
CL = LINK
LIBS = User32.lib Gdi32.lib
CFLAGS = -Zi -EHsc -c -W3
LFLAGS = /INCREMENTAL /MAP /DEBUG
OUT = build
TARGET = cnima.exe
OBJECTS = ${OUT}/win32_platform.o ${OUT}/drawing.o

${OUT}/${TARGET}: ${OBJECTS}
	${CL} ${LFLAGS} ${OUT}/win32_platform.o ${OUT}/drawing.o /OUT:${OUT}/${TARGET} ${LIBS}

$(OUT)/win32_platform.o: win32_platform.cpp
	${CC} ${CFLAGS} $? /Fo:$@ /Fd:$*.pdb

$(OUT)/drawing.o: drawing.cpp
	${CC} ${CFLAGS} $? /Fo:$@ /Fd:$*.pdb

run: ${OUT}/${TARGET}
	${OUT}/${TARGET}