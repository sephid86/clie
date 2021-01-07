#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <pthread.h>

#define SERV_IP "192.168.0.100" //서버 IP 주소
#define PORT 8282 //서버 포트 번호
#define BUFFER_SIZE 256 //송수신 버퍼 크기
#define CHAT_CODE "&($@" //채팅 코드(서버쪽과 일치해야함)

//변수는 스네이크 표기법으로 하고 이외엔 모두 파스칼 표기법으로 한다.
//상수는 k파스칼표기법으로 한다.
//전역변수는 g_스네이크 표기법으로 한다.
//괄호는 K&R 스타일로 한다.
//들여쓰기는 스페이스바 2칸 으로 한다.

int g_sockfd;

GtkTextBuffer *Buffer;

void EntryActivate();
void *OnRecv();

int main (int argc, char *argv[]) {
  //---Socket
  int len;
  struct sockaddr_in address;
  int result;

  g_sockfd = socket(AF_INET, SOCK_STREAM, 0);

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr(SERV_IP);
  address.sin_port = htons(PORT);
  len = sizeof(address);

  result = connect(g_sockfd, (struct sockaddr*)&address, len);

  if(result == -1) {
    perror("서버 접속에 실패했습니다.");
    exit(1);
  }

  //---window
  GtkWidget *Window, 
            *ScrolledWin, 
            *Entry, 
            *TextView,
            *Layout1;

  gtk_init (&argc, &argv);

  //---윈도우 설정
  Window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(Window), 320,240);
  gtk_window_set_title(GTK_WINDOW(Window), "클리에 0.1 for Linux");
  //gtk_container_set_border_width(GTK_CONTAINER(Window), 10);
  //gtk_widget_set_size_request(Window, 640, 480);
  g_signal_connect(Window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  //---컨테이너 설정
  Layout1 = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
  gtk_container_add(GTK_CONTAINER(Window), Layout1);

  //---여러줄 글상자
  TextView = gtk_text_view_new();
  Buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(TextView));
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(TextView), 1); //자동줄바꿈 켬 
  //gtk_widget_set_size_request(TextView, -1,350);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(TextView), 0);

  //---여러줄 글상자 스크롤
  ScrolledWin = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy 
    (GTK_SCROLLED_WINDOW(ScrolledWin), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC); 
  //수평 끔, 수직 자동
  gtk_container_add(GTK_CONTAINER(ScrolledWin), TextView);
  gtk_box_pack_start(GTK_BOX(Layout1),ScrolledWin,1,1,0);

  //---한줄 글상자
  Entry = gtk_entry_new();
  gtk_container_add(GTK_CONTAINER(Layout1), Entry);
  g_signal_connect(Entry, "activate", G_CALLBACK(EntryActivate), NULL);

  //---쓰레드 생성(소켓리시브)
  pthread_t thread_OnRecv;
  pthread_create(&thread_OnRecv, NULL, OnRecv, (void*)TextView);

  //---화면에 윈도우 출력
  gtk_widget_show_all(Window);
  gtk_main();
  return 0;
}

void EntryActivate(GtkEntry *Entry, GtkTextView *TextView) {
  const gchar *send_text;
  char send_buff[BUFFER_SIZE];

  send_text = gtk_entry_get_text(GTK_ENTRY(Entry));

  strcat(send_buff, CHAT_CODE);
  strncat(send_buff, send_text, BUFFER_SIZE - strlen(send_buff));

  write(g_sockfd, send_buff, BUFFER_SIZE);
  gtk_entry_set_text(GTK_ENTRY(Entry), "");
}

void *OnRecv(GtkTextView *TextView) {
  char recv_text[BUFFER_SIZE];
  GtkTextMark *mark;
  GtkTextIter end_iter;
  gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(Buffer), &end_iter);
  mark = gtk_text_buffer_create_mark(GTK_TEXT_BUFFER(Buffer), NULL, &end_iter, 1);

  while(1) {
    //---수신 버퍼를 비우고 수신함
    memset(&recv_text, 0, BUFFER_SIZE);
    read(g_sockfd, recv_text, BUFFER_SIZE);
    //    g_print("%s",recv_text);
    //---수신된 버퍼를 여러줄 글상자에 출력
    gtk_text_buffer_insert(GTK_TEXT_BUFFER(Buffer), &end_iter, recv_text,-1);
    //---스크롤을 마지막 버퍼로 이동함
    gtk_text_buffer_move_mark(GTK_TEXT_BUFFER(Buffer), mark, &end_iter);
    gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(TextView), mark);
  }
}

