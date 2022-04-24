# clie
프로그래밍 학습을 목적으로 만든 리눅스 GTK 채팅 프로그램입니다.
http://k-lint.net (도메인 비용을 지불하지 못해 접속 불가입니다. ㅠㅠ)

This is a chat program for learn GTK.

This is for Koreans.

--- 필요사항 ---

에코 기능이 가능한 server 가 필요합니다.

---
--- 컴파일 방법 ---

$ make

또는 ( or )

$ gcc -o clie *.c `pkg-config --cflags --libs gtk+-3.0` -export-dynamic

---

GTK 를 이용한 GUI chat client example. (use thread) 입니다.

보통 client 는 socket 을 recieve 하기 위한 thread 가 필요한데

GTK 를 이용한 GUI 환경에서는 이 socket recieve thread 가

error 를 발생시킵니다.

GTK 에서 thread 를 어떤 식으로 처리해야 하는지에 대한 예제 입니다.

간단하게 만든 예제 이므로 의견이 있으실 경우 알려주시면 감사하겠습니다.

