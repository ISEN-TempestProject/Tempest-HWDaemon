#include "GpsHandler.h"

using namespace std;

    
    GpsHandler::GpsHandler(){
        init();
        //lock();
    
    }
    
    GpsHandler::~GpsHandler(){
        kill();
        close();
    }
    

    GpsHandler *GpsHandler::get()
    {
        if(_gps == NULL)
        {
            _gps = new GpsHandler();
            _gps->start();
            cout << "Plop" << endl;
        }
        return _gps;
    }
    
    void GpsHandler::kill()
    {
        if (NULL != _gps)
        {
            _gps->stop();
            delete _gps;
            _gps = NULL;
        }
    }
    
    void GpsHandler::start(){
        t_acquire = new thread(&GpsHandler::acquire, this);
    }
    
    void GpsHandler::stop(){
        m_acquire = false;
        t_acquire->join();
    }

    
    float GpsHandler::latitude(){
        return _gps->m_gpsdata->fix.latitude;
    }
    
    float GpsHandler::longitude(){
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
    
    void GpsHandler::acquire(){
        m_acquire= true;
        while(m_acquire)
        {
            //wait for some data
            if (!m_gpsrec->waiting(50000000)){
                continue;
            }
            if ((m_gpsdata = m_gpsrec->read()) == NULL) {
                cerr << "Read error.\n" << endl;
                return;
            }
            cout<<"plop"<<endl;
        }
    }
    
    void GpsHandler::close(){
       m_gpsdata = m_gpsrec->stream(WATCH_DISABLE);
        //gps_close(m_gpsdata);
    }
    
GpsHandler *GpsHandler::_gps = NULL;
