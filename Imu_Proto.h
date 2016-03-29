#ifndef IMU_PROTO
#define IMU_PROTO
#include <stdint.h>
#include <list>
#include "Singleton.h"
#include <windows.h>
class CUart;
typedef CUart CUsart;

class CImu_Proto
{
public:
	struct RxdPkgTyp
	{
		uint16_t PackHead;
		uint8_t ByteNbr;
		uint8_t FramFunc;
		uint8_t AccX_H;
		uint8_t AccX_L;
		uint8_t AccY_H;
		uint8_t AccY_L;
		uint8_t AccZ_H;
		uint8_t AccZ_L;
		uint8_t GyrX_H;
		uint8_t GyrX_L;
		uint8_t GyrY_H;
		uint8_t GyrY_L;
		uint8_t GyrZ_H;
		uint8_t GyrZ_L;
		uint8_t MagX_H;
		uint8_t MagX_L;
		uint8_t MagY_H;
		uint8_t MagY_L;
		uint8_t MagZ_H;
		uint8_t MagZ_L;
		uint8_t CheckByte;
		uint8_t PackEnd;
	};
	static DWORD WINAPI run(LPVOID lpParameter);
	void update();
	uint8_t pop_RxdQue_();
	CImu_Proto();
	~CImu_Proto();
	float GyrAcc_xyz_[6];

private:
	static uint32_t TimeStamp_;
	/*static uint8_t RxdBuf_[sizeof(RxdPkgTyp)];*/
	/*static std::list<RxdPkgTyp> RxdQue_;*/
	
	CUsart* Usart_;
	void CUsart_Init();
};

typedef NormalSingleton<CImu_Proto> ImuProto;
#endif
