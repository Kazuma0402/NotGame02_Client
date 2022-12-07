#include <iostream>		// 入出力用
#include <WinSock2.h>	// WinSock用
#include <ws2tcpip.h>	// WinSock用

#pragma comment( lib, "ws2_32.lib" )

const unsigned short PORT = 8080;	// サーバのポート番号

int main()
{
	std::cout << "Server\n";
	// WinSockの初期化	WSAStartup()
	WSADATA  wsaData;

	// WinSock2.2 初期化処理
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		// エラー処理
	}

	// UDPソケットの作成	socket()
	SOCKET  sock;

	// UDPソケットの作成
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
	{
		// エラー
	}

	// ポート番号の割り当て	bind()
	// ソケットアドレス構造体
	SOCKADDR_IN  bindAddr;
	memset(&bindAddr, 0, sizeof(bindAddr));		// 0クリアで初期化
	bindAddr.sin_family = AF_INET;					// IPv4アドレス使用
	bindAddr.sin_port = htons(PORT);			// ポート番号8080指定	マジックナンバーはNG
	bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);	// すべての自分のNICが対象

	// ポート番号割り当て
	if (bind(sock, (SOCKADDR*)&bindAddr, sizeof(bindAddr)) == SOCKET_ERROR)
	{
		// エラー処理
	}

	// 常時稼働で無限ループ
	while (true)
	{
		// 整数値の受信	recvfrom()
		int recvCount;

		// 送信元のアドレス情報を格納するソケットアドレス構造体
		SOCKADDR_IN fromAddr;
		int fromlen = sizeof(fromAddr);

		// buffに受信した文字列を格納
		int ret = recvfrom(sock, (char*)&recvCount, sizeof(recvCount), 0, (SOCKADDR*)&fromAddr, &fromlen);
		if (ret == SOCKET_ERROR)
		{
			// エラー処理
		}

		// とても大事な処理 ... 多バイトデータだから	???()
		int count = ntohl( recvCount );

		std::cout << "Receive count : " << count << std::endl;

		// 整数値の個数分のxを生成
		char  buff[1024];
		for (int i = 0; i < count; i++)
		{
			buff[i] = 'x';
		}
		buff[count] = '\0';
		std::cout << "Send message : " << buff << std::endl;

		// 出来上がった文字列を送信	sendto()
		ret = sendto(sock, buff, sizeof(buff), 0, (SOCKADDR*)&fromAddr, fromlen);
	}

	// もし無限ループが終わったら
	// UDPソケット破棄		closesocket()
	if (closesocket(sock) == SOCKET_ERROR)
	{
		// エラー
	}

	// WinSock終了処理		WSACleanup()
	if (WSACleanup() != 0)
	{
		// エラー処理
	}

	return 0;	// 正常終了と信じて！
}