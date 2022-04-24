#실행파일 이름
TARGET=clie

#소스 파일
#SRCS= main.c a.c b.c
SRCS=*.c

#오브젝트 파일
OBJS=*.o

#컴파일러 이름
CC=gcc

#디버그 옵션
DEBUG=-g
OPTI=-O2
WARN=-Wall

#쓰레드 라이브러리
PTHREAD=-pthread

#파이프(컴파일중 비정상 종료 될경우 이 옵션을 빼주세요.)
PIPE=-pipe

#GTK 라이브러리
GTKLIB=`pkg-config --cflags --libs gtk+-3.0` -export-dynamic -no-pie

#최종옵션
TOTAL=$(GTKLIB)

all:
#	$(CC) $(OPTI) -c $(SRCS) $(TOTAL)
#	$(CC) $(OPTI) -o $(TARGET) $(OBJS) $(TOTAL)
	$(CC) $(OPTI) -o $(TARGET) $(SRCS) $(TOTAL)

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)

