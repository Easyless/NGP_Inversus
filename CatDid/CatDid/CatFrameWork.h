#pragma once

// 작성자 : 김태형
// 작성시작일 : 19.05.02

// 설명:
// 게임 프레임 워크
// 게임 오브젝트보관하거나 Draw, Update들을 제어하는 클래스

// 주의점:
// 게임 세부 로직은 최대한 오브젝트 단에서 처리합시다.
// 아직 Update나 Draw에서 매개변수로 쥐어줄 정보는 못정함

//class CatFramework
//{
//public:
//	static CatFramework* instance;
//	void Update(float deltaTime);
//	void Draw(PaintInfo info);
//};