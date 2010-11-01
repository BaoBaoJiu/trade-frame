/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/
#pragma once

#include <string>
#include <vector>
#include <sstream>

#include <LibSqlite/sqlite3.h>

#include <boost/shared_ptr.hpp>

#include <LibCommon/Delegate.h>

#include "TradingEnumerations.h"
#include "Instrument.h"
#include "Order.h"
#include "ProviderInterface.h"

// Multiple position records grouped together would be a multi-legged instrument, aka Combo
//   -- not sure how to construct this yet
//    -- a position consists of one or more legs, so a leg would be the atomic unit of composition
// once a position with multiple legs is in a portfolio, should be able to sum up the legs to see if rebalancing or adjustments are required
// A Portfolio should be a collection of position records, whether individual positions, or Combos
// check that orders for both sell side and buy side are not opened simultaneously
// a position is provider dependent, ie, only one provider per position
// Create Delegates so trade and market data updates propogate to combo and portfolio

class CPosition {
public:

  typedef boost::shared_ptr<CPosition> pPosition_t;

  typedef CProviderInterfaceBase::pProvider_t pProvider_t;

  typedef CInstrument::pInstrument_t pInstrument_t;
  typedef CInstrument::pInstrument_cref pInstrument_cref;

  typedef COrder::pOrder_t pOrder_t;
  typedef COrder::pOrder_ref pOrder_ref;

  typedef std::pair<const CPosition*, const CExecution&> execution_pair_t;
  typedef const execution_pair_t& execution_delegate_t;

  typedef sqlite3_int64 idPosition_t;

  CPosition( pInstrument_cref, pProvider_t pExecutionProvider, pProvider_t pDataProvider );
  CPosition( pInstrument_cref, pProvider_t pExecutionProvider, pProvider_t pDataProvider, const std::string& sNotes );
  CPosition( pInstrument_cref, pProvider_t pExecutionProvider, pProvider_t pDataProvider, 
    idPosition_t idPosition, sqlite3_stmt* pStmt );
  CPosition( idPosition_t idPosition, sqlite3_stmt* pStmt );
  ~CPosition(void);

  const std::string& Notes( void ) const { return m_sNotes; };
  void Append( std::string& sNotes ) { m_sNotes += sNotes; };

  pInstrument_cref GetInstrument( void ) const { return m_pInstrument; };
  double GetUnRealizedPL( void ) const { return m_dblUnRealizedPL; };
  double GetRealizedPL( void ) const { return m_dblRealizedPL; };
  double GetCommissionPaid( void ) const { return m_dblCommissionPaid; };

  COrder::pOrder_t PlaceOrder( // market
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide,
    unsigned long nOrderQuantity
    );
  COrder::pOrder_t PlaceOrder( // limit or stop
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide,
    unsigned long nOrderQuantity,
    double dblPrice1
    );
  COrder::pOrder_t PlaceOrder( // limit and stop
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide,
    unsigned long nOrderQuantity,
    double dblPrice1,  
    double dblPrice2
    );
  void CancelOrders( void );
  void ClosePosition( void );

  Delegate<const CPosition*> OnQuote;
  Delegate<const CPosition*> OnTrade;  // nothing useful currently
  Delegate<execution_delegate_t> OnExecution;
  Delegate<const CPosition*> OnCommission;

  void EmitStatus( std::stringstream& ssStatus );

  static void CreateDbTable( sqlite3* pDb );
  int BindDbKey( sqlite3_stmt* pStmt );
  int BindDbVariables( sqlite3_stmt* pStmt );
  static const std::string& GetSqlSelect( void ) { return m_sSqlSelect; };
  static const std::string& GetSqlInsert( void ) { return m_sSqlInsert; };
  static const std::string& GetSqlUpdate( void ) { return m_sSqlUpdate; };
  static const std::string& GetSqlDelete( void ) { return m_sSqlDelete; };

  void Set( pInstrument_cref, pProvider_t pExecutionProvider, pProvider_t pDataProvider );

protected:

  typedef CSymbolBase::quote_t quote_t;
  typedef CSymbolBase::trade_t trade_t;
  typedef CSymbolBase::greek_t greek_t;

  std::string m_sidExecutionAccount;
  std::string m_sidDataAccount;

  pProvider_t m_pExecutionProvider;
  pProvider_t m_pDataProvider;

  pInstrument_t m_pInstrument;
  std::string m_sInstrumentName;

  std::string m_sName;
  std::string m_sNotes;

  // all pending orders must be on the same side
  // pending orders need to cancelled in order to change sides
  // use an opposing position if playing both sides of the market
  OrderSide::enumOrderSide m_eOrderSidePending;  
  unsigned long m_nPositionPending;

  // indicates whether we are in a long or short position
  OrderSide::enumOrderSide m_eOrderSideActive;  
  unsigned long m_nPositionActive;

  // following value markers exclude commission
  double m_dblConstructedValue;  // based upon position trades  used for RealizedPL calcs, keeps accrueing
  double m_dblMarketValue;  // based upon market quotes

  // following value markers exclude commission
  double m_dblUnRealizedPL;  // based upon market quotes
  double m_dblRealizedPL;  // based upon position trades

  // contains total commissions
  double m_dblCommissionPaid;

  std::vector<pOrder_t> m_OpenOrders;  // active orders waiting to be executed or cancelled
  std::vector<pOrder_t> m_ClosedOrders;  // orders that have executed or have cancelled
  std::vector<pOrder_t> m_AllOrders;  // keeps track of all orders in case we have to search both lists

private:

  static const std::string m_sSqlCreate;
  static const std::string m_sSqlSelect;
  static const std::string m_sSqlInsert;
  static const std::string m_sSqlUpdate;
  static const std::string m_sSqlDelete;

  idPosition_t m_idPosition;
  std::string m_idPortfolio;

  bool m_bInstrumentAssigned;
  bool m_bExecutionAccountAssigned;
  bool m_bDataAccountAssigned;

  double m_dblMultiplier;

  void Construction( void );

  void HandleExecution( const std::pair<const COrder&, const CExecution&>& );
  void HandleCommission( const COrder& );

  void PlaceOrder( pOrder_t pOrder );

  void HandleQuote( quote_t );
  void HandleTrade( trade_t );
  void HandleGreek( greek_t );

};