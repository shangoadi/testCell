#ifndef CONTROLLER_HH
#define CONTROLLER_HH

//#define DEFAULT
#define TEST

#include <cstdint>

/* Congestion controller interface */

class Controller
{
private:
  bool debug_; /* Enables debugging output */

  #ifdef DEFAULT
  /* Add member variables here */
  double cwnd;
  double rtt;
  double last_ack_received;
  #endif


  #ifdef TEST
  double cwnd; //window size
  int reduced;
  int r1;
  int r2;

  /* Add member variables here */

  void cwnd_from_delay(int diff,uint64_t rtt);

  #endif
  
  
public:
  /* Public interface for the congestion controller */
  /* You can change these if you prefer, but will need to change
     the call site as well (in sender.cc) */

  /* Default constructor */
  Controller( const bool debug );

  /* Get current window size, in datagrams */
  unsigned int window_size( void );

  /* A datagram was sent */
  void datagram_was_sent( const uint64_t sequence_number,
			  const uint64_t send_timestamp );

  /* An ack was received */
  void ack_received( const uint64_t sequence_number_acked,
		     const uint64_t send_timestamp_acked,
		     const uint64_t recv_timestamp_acked,
		     const uint64_t timestamp_ack_received );

  /* How long to wait (in milliseconds) if there are no acks
     before sending one more datagram */
  unsigned int timeout_ms( void );
};

#endif
