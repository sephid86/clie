#include <gtk/gtk.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_IP "127.0.0.1" //서버 IP 주소
#define PORT 5959 //서버 포트 번호
#define BUFFER_SIZE 2048 //송수신 버퍼 크기

/*--------------------
  클리에 For Linux 0.1a
  (Clie For Linux. GTK socket client example 0.1a
  만든사람 : 배달하는사람
  sephid86@gmail.com - http://github.com/sephid86
  만든날짜 2022-04-25
  -OS Linux-
  -Release-
  0.1 GTK 소켓 기본기능 구현됨.
  0.1a GTK 쓰레드 안정성 향상.
  --------------------*/
//변수는 스네이크 표기법으로 하고 이외엔 모두 파스칼 표기법으로 한다.
//상수는 k파스칼표기법으로 한다.
//전역변수는 g_스네이크 표기법으로 한다.
//괄호는 K&R 스타일로 한다.
//들여쓰기는 스페이스바 2칸 으로 한다.

typedef struct {
  GtkWidget *TextView;
  char recv_str[BUFFER_SIZE];
} MyWidgetGroup;

int g_sockfd;
GtkTextBuffer *Buffer;

void EntryActivate();
void *SockRecv();
gboolean WidgetShowSafe();
gboolean ReadSocket();

//--- 이 소스는 아직 작업중 입니다. 완료되지 않아 작동하지 않습니다.
int main(int argc, char *argv[]) {
  //---Socket
  g_sockfd=socket(AF_INET, SOCK_STREAM, 0);

  GError * error = NULL;

  GSource *src;
  GIOChannel *channel;

  GSocketConnection * connection = NULL;
  GSocketClient * client = g_socket_client_new();

  connection = g_socket_client_connect_to_host(client, (gchar*)"localhost", 8282, NULL, &error);

  channel = g_io_channel_unix_new(g_sockfd);
  src = g_io_create_watch(channel, G_IO_IN);
  g_source_set_callback(src, (GSourceFunc)ReadSocket, NULL, NULL);

  gtk_init (&argc, &argv);

  //---윈도우 설정
  GtkWidget *Window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(Window), 320,240);
  gtk_window_set_title(GTK_WINDOW(Window), "클리에 0.1 for Linux");
  //gtk_container_set_border_width(GTK_CONTAINER(Window), 10);
  //gtk_widget_set_size_request(Window, 640, 480);

  //---레이아웃 컨테이너 설정
  GtkWidget *Layout1=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);

  //---여러줄 글상자 설정
  //GtkWidget *TextView=gtk_text_view_new();
  MyWidgetGroup *MyWidget=malloc(sizeof(MyWidgetGroup));
  MyWidget->TextView=gtk_text_view_new();
  Buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(MyWidget->TextView));
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(MyWidget->TextView), 1); 
  //gtk_widget_set_size_request(TextView, -1,350);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(MyWidget->TextView), 0);

  //---여러줄 글상자 스크롤 설정
  GtkWidget *ScrollTextview=gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy 
    (GTK_SCROLLED_WINDOW(ScrollTextview), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC); 

  //---한줄 글상자 설정
  GtkWidget *Entry = gtk_entry_new();

  //---컨테이너 등록
  gtk_container_add(GTK_CONTAINER(Window), Layout1);
  gtk_container_add(GTK_CONTAINER(ScrollTextview), MyWidget->TextView);
  gtk_box_pack_start(GTK_BOX(Layout1),ScrollTextview,1,1,0);
  gtk_container_add(GTK_CONTAINER(Layout1), Entry);

  //--- 이벤트 시그널 등록
  g_signal_connect(Window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(Entry, "activate", G_CALLBACK(EntryActivate), NULL);
  g_signal_connect(src, "G_IO_IN", G_CALLBACK(EntryActivate), NULL);

  //---쓰레드 생성(소켓리시브)
//  GThread *RecvThread=g_thread_new(NULL,(GThreadFunc)SockRecv,MyWidget);

  //---화면에 윈도우 출력
  gtk_widget_show_all(Window);
  gtk_main();
  return 0;
}

void EntryActivate(GtkEntry *Entry, GtkTextView *TextView) {
  const gchar *send_text=gtk_entry_get_text(GTK_ENTRY(Entry));

  write(g_sockfd, send_text, BUFFER_SIZE);
  gtk_entry_set_text(GTK_ENTRY(Entry), "");
}

void *SockRecv(MyWidgetGroup *MyWidget) {
  //GtkTextIter end_iter;
  //gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(Buffer), &end_iter);
  //GtkTextMark *mark = gtk_text_buffer_create_mark(GTK_TEXT_BUFFER(Buffer), NULL, &end_iter, 1);

  //char recv_str[BUFFER_SIZE];
  int tmp;
  while(1) {
    tmp=read(g_sockfd, MyWidget->recv_str, BUFFER_SIZE);
    if(tmp) {
      g_idle_add((GSourceFunc)WidgetShowSafe,MyWidget);
      //---수신된 버퍼를 여러줄 글상자에 출력
      //gtk_text_buffer_insert(GTK_TEXT_BUFFER(Buffer), &end_iter, recv_str,-1);
      //---스크롤을 마지막 버퍼로 이동함
      //gtk_text_buffer_move_mark(GTK_TEXT_BUFFER(Buffer), mark, &end_iter);
      //gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(TextView), mark);
    }
  }
}

gboolean WidgetShowSafe(MyWidgetGroup *MyWidget) {
  GtkTextIter end_iter;
  gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(Buffer), &end_iter);
  GtkTextMark *mark = gtk_text_buffer_create_mark(GTK_TEXT_BUFFER(Buffer), NULL, &end_iter, 1);

  //---수신된 버퍼를 여러줄 글상자에 출력
  gtk_text_buffer_insert(GTK_TEXT_BUFFER(Buffer), &end_iter, MyWidget->recv_str,-1);
  //---스크롤을 마지막 버퍼로 이동함
  gtk_text_buffer_move_mark(GTK_TEXT_BUFFER(Buffer), mark, &end_iter);
  gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(MyWidget->TextView), mark);
}

//위 소스코드를 살펴보면 SockRecv 에서 주석처리된 
//함수들이 WidgetShowSafe 함수로 이동한것을 알수 있습니다.
//즉 쓰레드에서 직접 GTK 함수를 호출하는 것은 위험합니다.
//쓰레드에서 GTK 함수를 호출하려면 g_idle_add 함수를 통해야 합니다.
//그리고 GUI 화면출력과 관련된 GTK 함수가 아니라면.
//예를 들어 widget 의 내용을 변경하는 GTK 함수 같은것들은
//쓰레드에서 직접 호출해도 왠만해선 문제가 없는것 같습니다.

