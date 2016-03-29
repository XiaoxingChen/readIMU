#include "Imu_Proto.h"
#include "CUart.h"
//uint8_t CImu_Proto::RxdBuf_[sizeof(CImu_Proto::RxdPkgTyp)];
CUsart Imu_Usart("COM3");
uint32_t CImu_Proto::TimeStamp_ = 0x000000;

CImu_Proto::CImu_Proto()
	:Usart_(&Imu_Usart)
{
	CUsart_Init();

	HANDLE hThread_1 = CreateThread(NULL, 0, run, 0, 0, NULL);
	CloseHandle(hThread_1);
}


CImu_Proto::~CImu_Proto()
{
}

//
// pop_RxdQue_()
//

uint8_t CImu_Proto::pop_RxdQue_()
{
	enum unpackMode_Type
	{
		CHECK_SIZE = 0,		//check left array size
		SRCH_HEAD = 1,		//search pack head
		CHECK_END = 2,		//check pack end
		RSERCH_TO_CHECK = 3,	//re-search pack head
		UNPACK = 4
	}unpackMode;

	unpackMode = RSERCH_TO_CHECK;
	static RxdPkgTyp tempPack;

	while (true)	//unpack state machine
	{
		/* CHECK_SIZE */
		if (unpackMode == CHECK_SIZE)	//to "break" or "SRCH_HEAD"
		{
			if (Imu_Usart.Get_cbInQue() < sizeof(tempPack))
				break;
			else
				unpackMode = SRCH_HEAD;
		}

		/* SRCH_HEAD */
		else if (unpackMode == SRCH_HEAD)	//to "SRCH_HEAD" or "CHECK_END"
		{
			Imu_Usart.PopRxdQueue((char*)&tempPack, 1);
			if (*((uint8_t*)&tempPack) == 0xA5)	unpackMode = CHECK_END;
		}

		/* CHECK_END */
		else if (unpackMode == CHECK_END)	// to "UNPACK" or "RSERCH_TO_CHECK"
		{
			Imu_Usart.PopRxdQueue((char*)&tempPack + 1, sizeof(tempPack)-1);
			if (tempPack.PackHead == 0x5AA5 && tempPack.PackEnd == 0xAA)
				unpackMode = UNPACK;
			else
				unpackMode = RSERCH_TO_CHECK;
		}

		/* RSERCH_TO_CHECK */
		else if (unpackMode == RSERCH_TO_CHECK)
		{
			uint8_t i;
			/* re-search the head in tempPack */
			for (i = 1; i < sizeof(tempPack); i++)
			{
				if (((uint8_t*)&tempPack)[i] == 0xA5) break;
			}
			char* leftPartAddr = ((char*)&tempPack) + sizeof(tempPack)-1 - i;
			uint16_t leftPartSize = sizeof(tempPack)-i;
			if (leftPartSize > 0)
			{
				/* check size */
				if (Imu_Usart.Get_cbInQue() < leftPartSize) break;

				/* check pack end */
				memmove(&tempPack, ((uint8_t*)&tempPack) + i, sizeof(tempPack)-i);
				Imu_Usart.PopRxdQueue(leftPartAddr, leftPartSize);
				if (tempPack.PackHead == 0x5AA5 && tempPack.PackEnd == 0xAA)
					unpackMode = UNPACK;
				else
					unpackMode = RSERCH_TO_CHECK;
			}
			else //leftPartSize == 0, no pack head in tempPack
				unpackMode = CHECK_SIZE;
		}

		/* UNPACK */
		else if (unpackMode == UNPACK) // to "break"
		{
			GyrAcc_xyz_[0] = (float)((uint16_t)tempPack.GyrX_H << 8) + tempPack.GyrX_L;
			GyrAcc_xyz_[1] = (float)((uint16_t)tempPack.GyrY_H << 8) + tempPack.GyrY_L;
			GyrAcc_xyz_[2] = (float)((uint16_t)tempPack.GyrZ_H << 8) + tempPack.GyrZ_L;

			GyrAcc_xyz_[3] = (float)((uint16_t)tempPack.AccX_H << 8) + tempPack.AccX_L;
			GyrAcc_xyz_[4] = (float)((uint16_t)tempPack.AccY_H << 8) + tempPack.AccY_L;
			GyrAcc_xyz_[5] = (float)((uint16_t)tempPack.AccZ_H << 8) + tempPack.AccZ_L;
			break;
		}
		else std::cout << "unpack enum error" << std::endl;
	}
	return 1;
}

//
//void run()
//
DWORD WINAPI CImu_Proto::run(LPVOID lpParameter)
{
	static uint32_t prevTime = 0;
	while (true)
	{
		while (prevTime == TimeStamp_) Sleep(1);
		prevTime = TimeStamp_;
		ImuProto::Instance()->pop_RxdQue_();
	}
}

//
// CUsart_Init
//
void CImu_Proto::CUsart_Init()
{
	if (Usart_ == NULL) std::cout << "Usart_ is NULL" << std::endl;
	Usart_->Init(115200);
}

//
//
//
void CImu_Proto::update()
{
	TimeStamp_++;

}
//end of file
