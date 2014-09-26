#include <string>

class Imu
{
public:
	Imu(const std::string& tty, const std::string& BB_UARTX);
	~Imu();

	void Query();

	float Heading(){
		float fRet = m_lastData.Psi;
		if(fRet<0.0)
			fRet+=2*3.14159265;
		return fRet*3.14159265/180.0;
	}

	float Pitch(){
		return m_lastData.Phi*3.14159265/180.0;//Maybe switch with roll?
	}

	float Roll(){
		return m_lastData.Theta*3.14159265/180.0;
	}


private:
	struct IMUDataHead{
		unsigned char Cmd;
		uint8_t Length;
	};

	struct IMUData{
		float Phi;//Roll?
		float Theta;//Pitch?
		float Psi;//Yaw
		float Q0;
		float Q1;
		float Q2;
		float Q3;
		float P;// rad/s
		float Q;// rad/s
		float R;// rad/s
		float AccX;// m/s2
		float AccY;// m/s2
		float AccZ;// m/s2
		float MagX;
		float MagY;
		float MagZ;
		float Reserved0;
		float Reserved1;
	};


	int m_tty;
	IMUData m_lastData;

};
