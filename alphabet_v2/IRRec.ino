


int recIR() {
 
  int received = 0;
  //Receive and decode the message from the IR receiver and return the value (received). 
  if (irrecv.decode(&results)) {
    
    // print() & println() can't handle printing long longs. (uint64_t)
    received = results.value;
    results.value = 0;
    irrecv.resume();  // Receive the next value
  }
  return received;
}
