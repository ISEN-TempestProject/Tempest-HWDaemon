#include "GpsHandler.h"

using namespace std;


    GpsHandler::GpsHandler(){
        init();
    }

    GpsHandler::~GpsHandler(){
        close();
    }


    GpsHandler *GpsHandler::get()
    {
        if(_gps == NULL)
        {
            _gps = new GpsHandler();
            _gps->lock();
            _gps->start();
        }
        return _gps;
    }

    void GpsHandler::kill()
    {

        if (NULL != _gps)
        {
            cout << "Killing GPS..." << endl;
            _gps->stop();
            delete _gps;
            _gps = NULL;
            cout << "GPS killed !" << endl;
        }
    }

    void GpsHandler::start(){
        pthread_create(&t_acquire, NULL, acquireWrapper, this);
    }

    void GpsHandler::stop(){
        cout << "Stopping GPS thread..." << endl;
        m_acquire = false;
        pthread_join(t_acquire, NULL);
        cout << "GPS thread stopped !" << endl;
    }


    double GpsHandler::latitude(){
        return _gps->m_gpsdata->fix.latitude;
    }

    double GpsHandler::longitude(){
        return _gps->m_gpsdata->fix.longitude;
    }


    void GpsHandler::init(){
        string host = "localhost";
        string port = "2947";
        cout << "Trying to connect to " << host << ":" << port << endl;
        m_gpsrec = new gpsmm((char*) host.c_str(),(char*) port.c_str());


        //connect to local GPSd
        if((m_gpsdata = m_gpsrec->stream(WATCH_ENABLE | WATCH_JSON)) == NULL ){
            cerr << "No GPSd running." << endl;
            exit(-1);
        }

        cout << "Gpsd Running" << endl;
    }

    void GpsHandler::lock(){
        cout << "Locking GPS..." << endl;
        while(!m_gpsrec->waiting(50000000) )
        {
            cout << "Waiting for GPS locking..." << endl;
        }
        cout << "GPS locked !" << endl;
    }

    void GpsHandler::acquire(){
        m_acquire= true;
        while(m_acquire)
        {
            //wait for some data
            if (!m_gpsrec->waiting(5000000)){
                continue;
            }
            if ((m_gpsdata = m_gpsrec->read()) == NULL) {
                cerr << "Read error.\n" << endl;
                return;
            }
            pthread_yield();
        }
    }

    void GpsHandler::close(){
       m_gpsdata = m_gpsrec->stream(WATCH_DISABLE);
        //gps_close(m_gpsdata);
    }



GpsHandler *GpsHandler::_gps = NULL;

void* acquireWrapper(void* obj){
    GpsHandler* ctrl = reinterpret_cast<GpsHandler*>(obj);
    ctrl->acquire();
    return nullptr;
}
