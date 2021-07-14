#include <stdio.h>
#include <atlstr.h>
#include <atlimage.h>
#include <windows.h>

bool getPosition(void);
void setPosition(void);
void pressButtons(void);

#define LEFTDOWN 2
#define LEFTUP 4
#define POSITION 3
#pragma warning(suppress : 5208)
typedef struct {
  POINT p[POSITION];
  int R = 0, G = 0, B = 0;
} POS;


bool multi_mode = FALSE;

HDC h_dc = GetDC(NULL);
CImage screen;
int const res[2] = { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };  // https://fosterahope.tistory.com/154
int const color_depth = GetDeviceCaps(h_dc, BITSPIXEL);  // 화면 색상 수
POS pos;


int main(int argc, char * argv[]) {
  screen.Create(res[0], res[1], color_depth, 0);  // 캡쳐 이미지 저장용

  while (TRUE) {
  	// init
  	printf_s("현재 접종 가능한 백신의 수가 1개 인가요? (y to yes) : ");
  	if (getchar() != 'y') {
  	  multi_mode = TRUE;
  	  printf_s("다중 백신 매크로 모드가 설정되었습니다.\n");
  	} getchar();
  	printf_s("스마트폰 미러링으로 진행하는 경우 폰에서 방해금지모드를 활성화해주세요!\n" \
  	         "낮은 안드로이드 버전에서는 UI 차이로 인해 동작하지 않을 수 있습니다.\n" \
  	         "듀얼 모니터 환경에서는 작동하지 않을 수 있습니다. 매크로 작동중에는 마우스를 움직이지 마세요!\n\n");

  	// 좌표 가져오기
  	if ((!getPosition()) ? printf_s("매크로 좌표가 지정되지 않았습니다. 새로 지정해주세요!\n") : \
  	                       printf_s("매크로 좌표 지정을 다시 할까요? (press y to excute) : ") && getchar() == 'y' && getchar()) {
      setPosition();
    }

  	// 매크로 작동
    printf_s("\n매크로 실행환경이 준비되면 아무키나 누르세요 : ");
    getchar();
  	printf_s("매크로 종료 옵션은 구현되어있지 않으므로, 강제 종료를 원할 시 CTRL+ALT+DEL 눌러서 로그오프 하세요.\n\n");
  	pressButtons();

  	// 수동으로 예약 완료 확인
  	if (printf_s("예약에 성공했는지 카카오톡에서 확인해주세요! (성공했다면 y) : ") && getchar() == 'y' && getchar()) {
      break;
    }
  	system("cls");
  }

  screen.ReleaseDC();
  ReleaseDC(NULL, h_dc);

  printf_s("매크로를 종료합니다.\n");
  system("pause");
  return 0;
}

bool getPosition(void) {
  printf_s("화면 해상도 : %d x %d\n", res[0], res[1]);
  FILE * fp;
  fopen_s(&fp, "pos.txt", "r");
  if (fp != NULL) {
    for (size_t i = 0; i < POSITION; i++) {
      fscanf_s(fp, "%d,%d\n", &pos.p[i].x, &pos.p[i].y);
    }
    fscanf_s(fp, "%d,%d,%d", &pos.R, &pos.G, &pos.B);
    fclose(fp);
  } else {
    return FALSE;
  }
  printf_s("refresh button : (%d, %d)\n", pos.p[0].x, pos.p[0].y);
  printf_s("book button : (%d, %d)\n", pos.p[1].x, pos.p[1].y);
  printf_s("ok button : (%d, %d) [%d, %d, %d]\n", pos.p[2].x, pos.p[2].y, pos.R, pos.G, pos.B);
  bool err = FALSE;
  for (size_t i = 0; i < POSITION; i++) {
    if (pos.p[i].x >= res[0] or pos.p[i].y >= res[1]) {
      err = TRUE;
    }
  }
  if (err) {
    printf_s("좌표는 화면 해상도 밖을 넘어갈 수 없습니다. 다시 설정하세요!\n");
    return FALSE;
  }
  if (pos.R == 255 && pos.G == 255 && pos.B == 255) {
    printf_s("확인 버튼은 흰색일 수 없습니다.\n");
    return FALSE;
  }
  return TRUE;
}

void setPosition(void) {
  POINT pt;

  printf_s("\n\n콘솔창을 포그라운드 최상위에 유지시킨 채로 마우스 커서를 카카오톡 예약창 새로고침 버튼 위치로 이동시킨 다음 키보드에서 아무키나 누르세요 : ");
  getchar();
  GetCursorPos(&pt); pos.p[0].x = pt.x; pos.p[0].y = pt.y;

  printf_s("\n\n마우스 커서를 당일예약 버튼 위치(최대한 아래쪽으로)로 이동시킨 다음 키보드에서 아무키나 누르세요 : ");
  getchar();
  GetCursorPos(&pt); pos.p[1].x = pt.x; pos.p[1].y = pt.y;

  COLORREF RGB = 0;
  int R = 0, G = 0, B = 0;  // channel values
  while (TRUE) {
    printf_s("\n\n마우스 커서를 확인(당일 방문해야 한다는 버튼; 카톡방 나가기 할 때 뜨는 이벤트의 확인버튼과 동일한 위치에 보통 존재)" \
      "버튼 위치로 이동시킨 다음 키보드에서 아무키나 누르세요 (확인버튼 글자에 딱 맞게 맞춰주세요! 파란색 글자가 인식돼야 정상적으로 등록됩니다.) : ");
    getchar();
    GetCursorPos(&pt); pos.p[2].x = pt.x; pos.p[2].y = pt.y;
    BitBlt(screen.GetDC(), 0, 0, res[0], res[1], h_dc, 0, 0, SRCCOPY);  // 화면 캡쳐
    RGB = screen.GetPixel(pt.x, pt.y);
  	R = GetRValue(RGB); G = GetGValue(RGB); B = GetBValue(RGB);
    printf_s("[%d, %d, %d]\n", R, G, B);
//    if (R == 50 && G == 136 && B == 214)
    if (!(R == 255 && G == 255 && B == 255)) {
      break;
    }
    printf_s("Error: 글자색이 정상적으로 인식되지 않았습니다. 확인버튼에 정확히 맞춰주세요! 재시도합니다.\n");
  }
  pos.R = R; pos.G = G; pos.B = B;

  FILE * fp;
  fopen_s(&fp, "pos.txt", "w+");
  if (fp != NULL) {
    for (size_t i = 0; i < POSITION; i++) {
      fprintf_s(fp, "%d,%d\n", pos.p[i].x, pos.p[i].y);
    }
    fprintf_s(fp, "%d,%d,%d\n", pos.R, pos.G, pos.B);
    fclose(fp);
  } else {
    printf_s("pos.txt를 작성하는 데 실패하였습니다.\n");
    system("pause");
    exit(1);
  }
}

// https://intunknown.tistory.com/334
// https://rabinx.tistory.com/37
void pressButtons(void) {
  COLORREF RGB = 0;
  int R = 0, G = 0, B = 0;  // channel values

  while (TRUE) {  // 당일 예약 버튼 누르기
    SetCursorPos(pos.p[0].x, pos.p[0].y);
    mouse_event(LEFTDOWN, 0, 0, 0, 0); mouse_event(LEFTUP, 0, 0, 0, 0);
    BitBlt(screen.GetDC(), 0, 0, res[0], res[1], h_dc, 0, 0, SRCCOPY);  // 화면 캡쳐
    RGB = screen.GetPixel(pos.p[1].x, pos.p[1].y);
//    R = GetRValue(RGB); G = GetGValue(RGB); B = GetBValue(RGB);
    if (GetGValue(RGB) == 229) {  // && R == 254 && B == 0) {
      SetCursorPos(pos.p[1].x, pos.p[1].y);
      mouse_event(LEFTDOWN, 0, 0, 0, 0); mouse_event(LEFTUP, 0, 0, 0, 0);
      printf_s(">>당일 예약 버튼 발견됨\n");
      break;
    }
  }

  while (multi_mode) {  // 백신 선택 창 예약하기 버튼 누르기
    BitBlt(screen.GetDC(), 0, 0, res[0], res[1], h_dc, 0, 0, SRCCOPY);  // 화면 캡쳐
    RGB = screen.GetPixel(pos.p[1].x, pos.p[1].y);
//    R = GetRValue(RGB); G = GetGValue(RGB); B = GetBValue(RGB);
    if (GetGValue(RGB) == 229) {  // && R == 254 && B == 0) {
      SetCursorPos(pos.p[1].x, pos.p[1].y);
      mouse_event(LEFTDOWN, 0, 0, 0, 0); mouse_event(LEFTUP, 0, 0, 0, 0);
      printf_s(">>백신 선택창 발견됨\n");
      break;
    }
  }

  while (TRUE) {  // 확인 버튼 누르기
    BitBlt(screen.GetDC(), 0, 0, res[0], res[1], h_dc, 0, 0, SRCCOPY);  // 화면 캡쳐
    RGB = screen.GetPixel(pos.p[2].x, pos.p[2].y);
//    R = GetRValue(RGB); G = GetGValue(RGB); B = GetBValue(RGB);
//    if (R == pos.R && G == pos.G && B == pos.B):
    if (!(GetGValue(RGB) == 255)) {
      SetCursorPos(pos.p[2].x, pos.p[2].y);
      mouse_event(LEFTDOWN, 0, 0, 0, 0); mouse_event(LEFTUP, 0, 0, 0, 0);
      printf_s(">>확인 버튼 발견됨\n");
      break;
    }
  }
}
