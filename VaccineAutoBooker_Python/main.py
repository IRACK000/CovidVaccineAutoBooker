#cython: language_level=3
# -*- coding: utf-8 -*-
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
### Alias : main & Last Modded : 2021.07.10. ###
Coded with Python 3.9 for Windows (CRLF) by IRACK000
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
import os
try:
    from PIL import ImageGrab
except ModuleNotFoundError:
    os.system("pip3 install pillow")
    from PIL import ImageGrab
try:
    import pyautogui as pag
except ModuleNotFoundError:
    os.system("pip3 install pyautogui")
    import pyautogui as pag
os.system("cls")


class Macro(object):
    """docstring for Macro."""

    def __init__(self):
        print("스마트폰 미러링으로 진행하는 경우 폰에서 방해금지모드를 활성화해주세요!\n"
              + "낮은 안드로이드 버전에서는 UI 차이로 인해 동작하지 않을 수 있습니다.\n"
              + "듀얼 모니터 환경에서는 작동하지 않을 수 있습니다. 매크로 작동중에는 마우스를 움직이지 마세요!\n")
        # 좌표 가져오기
        self.pos, self.rgb_ok = self.getPosition(pag.size())
        if input("매크로 좌표가 지정되지 않았습니다. 새로 지정해주세요! (press y to excute) : ") == 'y' if self.pos is None \
                else input("매크로 좌표 지정을 다시 할까요? (press y to excute) : ") == 'y':
            self.pos, self.rgb_ok = self.setPosition()

    def getPosition(self, resolution):
        print("화면 해상도 :", resolution)
        pos = []
        try:
            with open("pos.txt", 'r') as f:
                for i, line in enumerate(f.read().split('\n')):
                    p = line.split(',')
                    if i == 3:
                        rgb = (p[0], p[1], p[2])
                        break
                    pos.append(pag.Point(int(p[0]), int(p[1])))
        except Exception as e:
            print(e)
            return None, None

        print("refresh button : ", pos[0])
        print("book button : ", pos[1])
        print("ok button : ", pos[2], rgb)
        err = False
        for p in pos:
            if p.x >= resolution[0] or p.y >= resolution[1]:
                err = True
        if err:
            print("좌표는 화면 해상도 밖을 넘어갈 수 없습니다. 다시 설정하세요!")
            return None, None
        if rgb[0] == 255 and rgb[1] == 255 and rgb[2] == 255:
            print("확인 버튼은 흰색일 수 없습니다.")
            return None, None
        return tuple(pos), rgb

    def setPosition(self):
        input("\n\n콘솔창을 포그라운드 최상위에 유지시킨 채로 마우스 커서를 카카오톡 예약창 새로고침 버튼 위치로 이동시킨 다음 키보드에서 아무키나 누르세요 : ")
        pos_refresh = pag.position()
        input("\n\n마우스 커서를 당일예약 버튼 위치(최대한 아래쪽으로)로 이동시킨 다음 키보드에서 아무키나 누르세요 : ")
        pos_book = pag.position()
        while True:
            input("\n\n마우스 커서를 확인(당일 방문해야 한다는 버튼; 카톡방 나가기 할 때 뜨는 이벤트의 확인버튼과 동일한 위치에 보통 존재)"
                  + "버튼 위치로 이동시킨 다음 키보드에서 아무키나 누르세요 (확인버튼 글자에 딱 맞게 맞춰주세요! 파란색 글자가 인식돼야 정상적으로 등록됩니다.) : ")
            pos_ok = pag.position()
            rgb = ImageGrab.grab().getpixel(pos_ok)
            print(rgb)
            # if rgb[0] == 50 and rgb[1] == 136 and rgb[2] == 214:
            if not (rgb[0] == 255 and rgb[1] == 255 and rgb[2] == 255):
                break
            print("Error: 글자색이 정상적으로 인식되지 않았습니다. 확인버튼에 정확히 맞춰주세요! 재시도합니다.")
        pos = (pos_refresh, pos_book, pos_ok)

        with open("pos.txt", 'w') as f:
            for p in pos:
                f.write("%d,%d\n" % (p.x, p.y))
            f.write("%d,%d,%d\n" % (rgb[0], rgb[1], rgb[2]))

        print("좌표 지정이 끝났습니다.\n")
        return pos, rgb

    def pressButtons(self):
        pos = self.pos

        while True:
            pag.moveTo(pos[0].x, pos[0].y)
            pag.click()
            rgb = ImageGrab.grab().getpixel(pos[1])
            if rgb[1] == 229:  # and rgb[0] == 254:  # and rgb[2] == 0:
                pag.moveTo(pos[1].x, pos[1].y)
                pag.click()
                print(">>당일 예약 버튼 발견됨")
                break

        while True:
            rgb = ImageGrab.grab().getpixel(pos[2])
            # if rgb[0] == rgb_ok[0] and rgb[1] == rgb_ok[1]:
            if not rgb[1] == 255:
                pag.moveTo(pos[2].x, pos[2].y)
                pag.click()
                print(">>확인 버튼 발견됨")
                break


class Macro_MultiVC(Macro):
    """docstring for Macro_MultiVC."""

    def __init__(self):
        print("다중 백신 매크로 모드가 설정되었습니다.")
        super(Macro_MultiVC, self).__init__()

    def pressButtons(self):
        pos = self.pos

        while True:  # 당일 예약 버튼 누르기
            pag.moveTo(pos[0].x, pos[0].y)
            pag.click()
            rgb = ImageGrab.grab().getpixel(pos[1])
            if rgb[1] == 229:  # and rgb[0] == 254:  # and rgb[2] == 0:
                pag.moveTo(pos[1].x, pos[1].y)
                pag.click()
                print(">>당일 예약 버튼 발견됨")
                break

        while True:  # 백신 선택 창 예약하기 버튼 누르기
            rgb = ImageGrab.grab().getpixel(pos[1])
            if rgb[1] == 229:  # and rgb[0] == 254:  # and rgb[2] == 0:
                pag.moveTo(pos[1].x, pos[1].y)
                pag.click()
                print(">>백신 선택창 발견됨")
                break

        while True:  # 확인 버튼 누르기
            rgb = ImageGrab.grab().getpixel(pos[2])
            # if rgb[0] == rgb_ok[0] and rgb[1] == rgb_ok[1]:
            if not rgb[1] == 255:
                pag.moveTo(pos[2].x, pos[2].y)
                pag.click()
                print(">>확인 버튼 발견됨")
                break


if __name__ == '__main__':
    while True:
        macro = Macro() if input("현재 접종 가능한 백신의 수가 1개 인가요? (y to yes) : ") == 'y' else Macro_MultiVC()

        # 매크로 작동
        input("매크로 실행환경이 준비되면 아무키나 누르세요 : ")
        print("매크로 종료 옵션은 구현되어있지 않으므로, 강제 종료를 원할 시 CTRL+ALT+DEL 눌러서 로그오프 하세요.")
        macro.pressButtons()

        # 수동으로 예약 완료 확인
        if input("예약에 성공했는지 카카오톡에서 확인해주세요! (성공했다면 y) : ") == 'y':
            break
        os.system("cls")

    print("매크로를 종료합니다.")
    os.system("pause")
