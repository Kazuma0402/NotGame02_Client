// ChatServer.cpp : アプリケーションのエントリ ポイントを定義します。
//
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "framework.h"
#include "ChatClient.h"
#include "resource.h"
#include <string>
#include <regex>

#pragma comment( lib, "ws2_32.lib" )


// グローバル変数
HINSTANCE hInst;
HWND hMessageEdit;
HWND hSendMessageEdit;
HWND hIpAddressEdit;
HWND hPortEdit;
HWND hSendNameEdit;
const unsigned short SERVERPORT = 8080;
const UINT_PTR TIMERID = 1000;

std::string message;   // チャット欄にセットする文字列
SOCKET sock;


// ダイアログプロシージャ
BOOL CALLBACK BindDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
BOOL CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

// このコード モジュールに含まれる関数の宣言を転送します:
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int  nCmdShow)
{
    hInst = hInstance;

    //   if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_BINDDIALOG), NULL, (DLGPROC)BindDlgProc) == IDOK)
    {
        // ダイアログ
        DialogBox(hInstance, MAKEINTRESOURCE(IDD_CHATSERVERDIALOG), NULL, (DLGPROC)DlgProc);
    }

    return (int)0;
}

BOOL CALLBACK BindDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_COMMAND:
        switch (LOWORD(wp))
        {
            // 送信ボタン押下時
        case IDOK:
            // 送信メッセージを取得

            EndDialog(hDlg, IDOK);
            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }


        return FALSE;
    }

    return FALSE;
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    char buff[1024];
    char ipAddr[256];
    char name[16];

    std::string str;
    std::regex re("N0NZ3ypzgRzm");
    std::smatch m;

    int port;
    char portstr[256];
    u_long arg = 0x01;

    int ret;
    int ret2;

    SOCKADDR_IN fromAddr;
    int fromlen = sizeof(fromAddr);

    SOCKADDR_IN toAddr;
    int tolen = sizeof(toAddr);

    switch (msg)
    {
    case WM_INITDIALOG:
        // 各コントロールのハンドル取得
        hMessageEdit = GetDlgItem(hDlg, IDC_MESSAGEEDIT);
        hSendMessageEdit = GetDlgItem(hDlg, IDC_EDIT2);
        hIpAddressEdit = GetDlgItem(hDlg, IDC_IPADDRESS1);
        hPortEdit = GetDlgItem(hDlg, IDC_PORTEDIT);
        hSendNameEdit = GetDlgItem(hDlg, IDC_NAMEEDIT);


        // WinSock初期化
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);

        // ソケット作成
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock > 0)
        {
			message.append("参加しました\r\n");
        }

        // ノンブロッキングソケットに設定
        ioctlsocket(sock, FIONBIO, &arg);

        // bind
        SOCKADDR_IN bindAddr;
        memset(&bindAddr, 0, sizeof(bindAddr));
        bindAddr.sin_family = AF_INET;
        bindAddr.sin_port = htons(SERVERPORT);
        bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);	// すべての自分のNICが対象
        if (ret = bind(sock, (SOCKADDR*)&bindAddr, sizeof(bindAddr)) == 0)
        {
        }

        // タイマーセット
        SetTimer(hDlg, TIMERID, 100, NULL);

		//サーバーのパソコンのアドレスで固定
		SetWindowTextA(hIpAddressEdit, "192.168.43.69");
		SetWindowTextA(hPortEdit, ("8080"));
		SetWindowTextA(hSendNameEdit, ("NoName"));
		//---------------------------------------------------

		GetWindowTextA(hSendMessageEdit, buff, 1024);

		// 名前の取得
		GetWindowTextA(hSendNameEdit, name, 16);
		// 宛先IPアドレスの取得
		GetWindowTextA(hIpAddressEdit, ipAddr, 256);
		// 宛先のポート番号の取得
		port = GetDlgItemInt(hDlg, IDC_PORTEDIT, FALSE, FALSE);

		memset(&toAddr, 0, sizeof(toAddr));
		toAddr.sin_family = AF_INET;
		inet_pton(AF_INET, ipAddr, &toAddr.sin_addr.s_addr);
		toAddr.sin_port = htons(port);
		sendto(sock, buff, sizeof(buff), 0, (SOCKADDR*)&toAddr, tolen);
		sendto(sock, name, sizeof(name), 0, (SOCKADDR*)&toAddr, tolen);

		// チャット欄に文字列セット
		SetWindowTextA(hMessageEdit, message.c_str());

		// 送信メッセージ入力欄をクリア
		SetWindowTextA(hSendMessageEdit, "");

        // タイマーセット
        SetTimer(hDlg, TIMERID, 100, NULL);

        return TRUE;
    case WM_TIMER:
        // 受信
        ret = recvfrom(sock, (char*)buff, sizeof(buff), 0, (SOCKADDR*)&fromAddr, &fromlen);
        ret2 = recvfrom(sock, (char*)name, sizeof(name), 0, (SOCKADDR*)&fromAddr, &fromlen);
        if (ret < 0)// || ret2 < 0)
        {
            if (WSAGetLastError() != WSAEWOULDBLOCK)
            {
                // エラー
                exit(1);
            }
            return TRUE;
        }
        else
        {
            str = buff;
            //文字検索
            if (std::regex_search(str, m, re))
            {
                //〇〇が退出しましたと表示
                message.append(name);
                message.append("が退出しました");
                message.append("\r\n");
                //文字の表示
                SetWindowTextA(hMessageEdit, message.c_str());

            }
            else
            {
                message.append(name);
                message.append(":");
                message.append(buff);
                message.append("\r\n");
                //文字の表示
                SetWindowTextA(hMessageEdit, message.c_str());
            }
        }
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wp))
        {
            // 送信ボタン押下時
        case IDC_SENDBUTTON:
            // 送信メッセージを取得
            GetWindowTextA(hSendMessageEdit, buff, 1024);
            // 名前の取得
            GetWindowTextA(hSendNameEdit, name, 16);
            // 宛先IPアドレスの取得
            GetWindowTextA(hIpAddressEdit, ipAddr, 256);
            // 宛先のポート番号の取得
            port = GetDlgItemInt(hDlg, IDC_PORTEDIT, FALSE, FALSE);

            memset(&toAddr, 0, sizeof(toAddr));
            toAddr.sin_family = AF_INET;
            inet_pton(AF_INET, ipAddr, &toAddr.sin_addr.s_addr);
            toAddr.sin_port = htons(port);
            ret = sendto(sock, buff, sizeof(buff), 0, (SOCKADDR*)&toAddr, tolen);
            ret2 = sendto(sock, name, sizeof(name), 0, (SOCKADDR*)&toAddr, tolen);


            // buffをチャット欄に追加
            message.append(name);
            message.append(":");
            message.append(buff);
            message.append("\r\n");

            // チャット欄に文字列セット
            SetWindowTextA(hMessageEdit, message.c_str());

            // 送信メッセージ入力欄をクリア
            SetWindowTextA(hSendMessageEdit, "");

            return TRUE;

        //閉じるボタンを押したら
        case IDCANCEL:
            char buff2[1024] = "N0NZ3ypzgRzm";
            // 名前の取得
            GetWindowTextA(hSendNameEdit, name, 16);
            // 宛先IPアドレスの取得
            GetWindowTextA(hIpAddressEdit, ipAddr, 256);
            // 宛先のポート番号の取得
            port = GetDlgItemInt(hDlg, IDC_PORTEDIT, FALSE, FALSE);
            
            //初期化
            memset(&toAddr, 0, sizeof(toAddr));
            //アドレスファミリ(IPv4)
            toAddr.sin_family = AF_INET;
            //テキストから数値への変換(IPv4) ※AF_INET6ならIPv6
            inet_pton(AF_INET, ipAddr, &toAddr.sin_addr.s_addr);
            //ホストバイトオーダーをネットワークバイトオーダーに変換
            toAddr.sin_port = htons(port);
            //データの送信
            ret = sendto(sock, buff2, sizeof(buff2), 0, (SOCKADDR*)&toAddr, tolen);
            ret2 = sendto(sock, name, sizeof(name), 0, (SOCKADDR*)&toAddr, tolen);


            KillTimer(hDlg, TIMERID);
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        return FALSE;
    }

    return FALSE;
}
