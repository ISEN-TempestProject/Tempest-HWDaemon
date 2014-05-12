#ifndef GPS_HANDLER_H
#define GPS_HANDLER_H

	#include <iostream>
	#include <cstdlib>
	#include <pthread.h>
    #include <string>
    #include <unistd.h>
	#include <libgpsmm.h>
	#include <math.h>


    class GpsHandler
    {

        private : //Singleton related
            static GpsHandler *_gps;
            GpsHandler();
            ~GpsHandler();


        public:
            /*
            *   Get the Gps singleton instance.
            */
           static GpsHandler *get();

           /*
            *   Get the Gps singleton instance.
            */
           static void kill();



            /*
            *   Get latitude from Gps
            */
            double latitude();

            /*
            *   Get longitude from Gps
            */
            double longitude();

            void acquire();

        private:
            gpsmm *m_gpsrec;
		    gps_data_t *m_gpsdata;
    		pthread_t t_acquire;
    		bool m_acquire;
            /*
            *   Initialize Gps daemon
            */
            void init();

            void lock();

            //start and stop thread properly
            void start();
            void stop();


            /*
            *   Acquire data while m_acquire is true.
            *   Threaded by constructor.
            *   Initializes m_acquire to true on call.
            */

            void close();
    };

            static void* acquireWrapper(void* obj);

#endif
