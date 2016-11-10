#include <iostream>

#include "controller.hh"
#include "timestamp.hh"

using namespace std;

// #define DEFAULT
// #define FIXED_WINDOW
// #define AIMD
//#define DELAY_TRIGGER


#define TEST


// Shared AIMD Delay-trigger constants
#define AI (1.0)
#define MD (1.0 / 2)
#define TIMEOUT (75)

// FIXED_WINDOW constants
#define FIXED_WINDOW_SIZE (26)

// Delay-trigger constants
#define DELAY_THRESHOLD (1000)


#ifdef DEFAULT
/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug ), cwnd( 1.0 ), rtt( 0.0 ), last_ack_received( 0 )
{}
#endif

#ifdef TEST
int acks = 0;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug ), cwnd(1), reduced(0), r1(0), r2(0)
{

}
#endif


/* Get current window size, in datagrams */
unsigned int Controller::window_size( void )
{
  
#ifdef FIXED_WINDOW
  /* Default: fixed window size of 100 outstanding datagrams */
  int the_window_size = FIXED_WINDOW_SIZE;

  if ( debug_ )
    {
      cerr << "At time " << timestamp_ms()
	 << " window size is " << the_window_size << endl;
    }
  return the_window_size;
#endif

#ifdef AIMD
  return (unsigned int) cwnd;
#endif

#ifdef DELAY_TRIGGER
  return (unsigned int) cwnd;
#endif

#ifdef TEST
  int the_window_size = cwnd;
  return the_window_size;
#endif
}

/* A datagram was sent */
void Controller::datagram_was_sent( const uint64_t sequence_number,
				    /* of the sent datagram */
				    const uint64_t send_timestamp )
                                    /* in milliseconds */
{
  /* Default: take no action */

  if ( debug_ ) {
    cerr << "At time " << send_timestamp
	 << " sent datagram " << sequence_number << endl;
  }

}

/* An ack was received */
void Controller::ack_received( const uint64_t sequence_number_acked,
			       /* what sequence number was acknowledged */
			       const uint64_t send_timestamp_acked,
			       /* when the acknowledged datagram was sent (sender's clock) */
			       const uint64_t recv_timestamp_acked,
			       /* when the acknowledged datagram was received (receiver's clock)*/
			       const uint64_t timestamp_ack_received )
                               /* when the ack was received (by sender) */
{
  /* Default: take no action */

  if ( debug_ ) {
    cerr << "At time " << timestamp_ack_received
	 << " received ack for datagram " << sequence_number_acked
	 << " (send @ time " << send_timestamp_acked
	 << ", received @ time " << recv_timestamp_acked << " by receiver's clock)"
	 << endl;
  }

#ifdef TEST
	int diff = recv_timestamp_acked - send_timestamp_acked;
	uint64_t rtt = timestamp_ack_received - send_timestamp_acked;
	cwnd_from_delay(diff,rtt);
#endif  

#ifdef AIMD
  // Calculate RTT
  const uint64_t this_rtt = timestamp_ack_received - send_timestamp_acked;

  if (this_rtt > rtt)
    {
      cwnd = MD * cwnd;
      if (cwnd < 1.0)
      {
	cwnd = 1.0;
      }
    }
  else
    {
      cwnd = cwnd + (AI / ((unsigned int) cwnd));
    }
#endif

#ifdef DELAY_TRIGGER
  const uint64_t this_rtt = timestamp_ack_received - send_timestamp_acked;
  if (this_rtt > DELAY_THRESHOLD)
    {
      cwnd = MD * cwnd;
      if (cwnd < 1.0)
	{
	  cwnd = 1.0;
	}
    }
  else
    {
      cwnd = cwnd + (AI / ((unsigned int) cwnd));
    }
#endif
  
}  


/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int Controller::timeout_ms( void )
{
  return TIMEOUT; /* timeout of one second */
}


#ifdef TEST
void Controller::cwnd_from_delay( int diff, uint64_t rtt)
{
  
       if (diff > 400){
		r1=0;
		r2=0.9;
		cwnd = cwnd + ((3.14*r1*r1*(1)) - (3.14*r2*r2*(1)));
		if(cwnd<0)
		  cwnd=0;				
       }else if ((diff > 350 && reduced<=4)){
	 //cwnd = cwnd/2;
		reduced=5;
		r1=0;		
		r2=0.8;
		cwnd = cwnd + ((3.14*r1*r1*(1)) - (3.14*r2*r2*(1)));
		if(cwnd<0)
		  cwnd=0;						
       }else if ((diff > 300 && reduced<=3)){
	 //cwnd = cwnd/2;
		reduced=4;
		r1=0;		
		r2=0.7;
		cwnd = cwnd + ((3.14*r1*r1*(1)) - (3.14*r2*r2*(1)));
		if(cwnd<0)
		  cwnd=0;				
       }else if ((diff > 200 && reduced<=2)){
	 //cwnd = cwnd/2;
		reduced=3;
		r1=0.1;
		r2=0.4;
		cwnd = cwnd + ((3.14*r1*r1*(1)) - (3.14*r2*r2*(1)));
		if(cwnd<0)
		  cwnd=0;
       }else if ((diff > 100 && reduced<=1)){
	 //cwnd = cwnd/2;
		reduced=2;
      		r1=0.2;		
		r2=0.3;
		cwnd = cwnd + ((3.14*r1*r1*(1)) - (3.14*r2*r2*(1)));
		if(cwnd<0)
		  cwnd=0;
       }else if ((diff > 50 && reduced<=0)){
	 //cwnd = cwnd/2;
		reduced=1;
		r1=0.9;				
		r2=0.1;
		cwnd = cwnd + ((3.14*r1*r1*(1)) - (3.14*r2*r2*(1)));
		if(cwnd<0)
		  cwnd=0;
	  
	}else if (rtt<50){
	 r1=1;				
	 r2=0.0;
	 cwnd = cwnd + ((3.14*r1*r1*(1)) - (3.14*r2*r1*(1)));
	  if(cwnd<0)
	    cwnd=0;
	  reduced = 0;
	}


}
#endif
