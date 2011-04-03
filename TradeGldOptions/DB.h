/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

//#include <OUSQL/Session.h>
//#include <OUSqlite/ISqlite3.h>
#include <TFTrading/Database.h>

#include <TFTrading/KeyTypes.h>

#include <TFTrading/TableDefs.h>

class CDB
{
public:

  CDB(void);
  ~CDB(void);

  void Open( const std::string& sDbName );
  void Close( void );

  bool IsOpen( void ) { return m_bOpened; };

  static ou::tf::keytypes::idPortfolio_t& PortfolioId( void ) { return m_sPortfolioId; };

  void CreatePortfolioAndPositionRecords( 
    const ou::tf::keytypes::idPortfolio_t& idPortfolio, 
    const ou::tf::keytypes::idInstrument_t& idUnderlying, const ou::tf::keytypes::idInstrument_t& idOption,
    const ou::tf::keytypes::idAccount_t& idExecutionAccount, const ou::tf::keytypes::idAccount_t& idDataAccount
  );
  bool LoadPortfolioAndPositions( const ou::tf::keytypes::idPortfolio_t& id, ou::tf::CPortfolio::pPortfolio_t& pPortfolio, 
    ou::tf::CPosition::pPosition_t& pPosUnderlying, ou::tf::CPosition::pPosition_t& pPosOption 
    );
  bool LoadUnderlying( const ou::tf::keytypes::idInstrument_t& id, ou::tf::CInstrument::pInstrument_t& pInstrument );
  void SaveInstrument( ou::tf::CInstrument::pInstrument_t& pInstrument );
  bool LoadOptions( const ou::tf::keytypes::idInstrument_t& idUnderlying, boost::uint16_t nYear, boost::uint16_t nMonth, boost::uint16_t nDay ); // uses OnNewInstrument

  typedef FastDelegate1<ou::tf::CInstrument::pInstrument_t> OnNewInstrumentHandler_t;
  void SetOnNewInstrumentHandler( OnNewInstrumentHandler_t function ) {
    OnNewInstrument = function;
  }

protected:
private:

  bool m_bOpened;

  static std::string m_sPortfolioId;

  ou::db::CSession m_session;

  void Populate( void );

  OnNewInstrumentHandler_t OnNewInstrument;
};
