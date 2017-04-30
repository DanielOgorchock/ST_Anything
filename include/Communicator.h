#ifndef ST_COMMUNICATOR_H
#define ST_COMMUNICATOR_H


namespace st
{
    class Communicator
    {
        private:


        public:
            
            virtual ~Communicator();

            virtual void init()=0;
            
            virtual char* run()=0;

            virtual void send(const char* message, uint16_t size)=0;


    };
}



#endif
