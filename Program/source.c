#include<stdio.h>
#include<Windows.h>

#define SIZE 10

int main()
{
#pragma region 포인터 배열
	
	const char* dialog[SIZE];

	dialog[0] = "어서 오세요";
	dialog[1] = "안녕하세요 의뢰할 게 있어서요";
	dialog[2] = "그게 무엇인가요?";
	dialog[3] = "요즘 주변에 이상한 일이 일어나고 있어요";
	dialog[4] = "자세히 말해주세요";
	dialog[5] = "제 옆 가게에서 부터 시작된 도난 사건인데, 제 가게를 제외한 가게가 전부 도난당했어요";
	dialog[6] = "흠... 이상하네요";
	dialog[7] = "왜 제 가게는 도난을 당하지 않았죠?";
	dialog[8] = "제가 지금부터 조사해보겠습니다";
	dialog[9] = "감사합니다";

	// 0x0000 : 이전에 누른 적이 없고 호출 시점에도 눌려있지 않은 상태

	// 0x0001 : 이전에 누른 적이 있고 호출 시점에는 눌려있지 않은 상태

	// 0x8000 : 이전에  누른 적이 없고 호출 시점에는 눌려있는 상태

	// 0x8001 : 이전에 누른 적이 있고 호출 시점에도 눌려있는 상태

	while (1)
	{
		if (GetAsyncKeyState(VK_SPACE) & 0x0001)
		{
			printf("enter...\n");

		}
	}
	

#pragma endregion




	return 0;
}