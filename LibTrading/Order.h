#pragma once

//#include "ProviderInterface.h"
#include "TradingEnumerations.h"
#include "Instrument.h"

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <string>

class COrder {
public:
  COrder(  // market 
    CInstrument *instrument, // not deleted here, need a smart pointer
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderAmount = 100,
    ptime dtOrderSubmitted = not_a_date_time
    );
  COrder(  // limit, stop 
    CInstrument *instrument, // not deleted here, need a smart pointer
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    double dblPrice1,  
    unsigned long nOrderAmount = 100,
    ptime dtOrderSubmitted = not_a_date_time
    );
  COrder(  // limit stop
    CInstrument *instrument, // not deleted here, need a smart pointer
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    double dblPrice1,  
    double dblPrice2,
    unsigned long nOrderAmount = 100,
    ptime dtOrderSubmitted = not_a_date_time
    );
  virtual ~COrder(void);
  CInstrument *GetInstrument( void ) { return m_pInstrument; };
  const char *GetOrderSideName( void ) { return OrderSide::Name[ m_eOrderSide ]; };
  unsigned long GetQuantity( void ) { return m_nOrderQuantity; };
  OrderType::enumOrderType GetOrderType( void ) { return m_eOrderType; };
  double GetPrice1( void ) { return m_dblPrice1; };  // need to validate this on creation
  double GetPrice2( void ) { return m_dblPrice2; };

protected:
  //std::string m_sSymbol;
  CInstrument *m_pInstrument;
  unsigned long m_nOrderId;
  OrderType::enumOrderType m_eOrderType;
  OrderSide::enumOrderSide m_eOrderSide;
  unsigned long m_nOrderQuantity;
  unsigned long m_nOrderQuantityRemaining;
  double m_dblPrice1;  // for limit
  double m_dblPrice2;  // for stop

  //CProviderInterface *m_pProvider;  // associated provider
  ptime m_dtOrderSubmitted;
  ptime m_dtOrderCancelled;
  ptime m_dtOrderExecuted;
private:
  COrder(void);
};