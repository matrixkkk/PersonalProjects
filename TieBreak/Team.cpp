#include "Team.h"
#include <windows.h>

void CPlayerList::Init()
{
    m_PlayerList.insert( std::make_pair( MBC,     new CDBPlayer( 70, 10, 70, "mbc1",       "BMP/mbc1.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( MBC,     new CDBPlayer( 70, 10, 70, "mbc2",       "BMP/mbc2.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( MBC,     new CDBPlayer( 70, 10, 70, "mbc3",       "BMP/mbc3.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( HANBIT,  new CDBPlayer( 70, 10, 70, "hanbit1",    "BMP/hanbit1.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( HANBIT,  new CDBPlayer( 70, 10, 70, "hanbit1",    "BMP/hanbit2.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( HANBIT,  new CDBPlayer( 70, 10, 70, "hanbit1",    "BMP/hanbit3.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( KTF,     new CDBPlayer( 70, 10, 70, "ktf1",       "BMP/ktf1.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( KTF,     new CDBPlayer( 70, 10, 70, "ktf2",       "BMP/ktf2.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( KTF,     new CDBPlayer( 70, 10, 70, "ktf3",       "BMP/ktf3.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( T1,      new CDBPlayer( 70, 10, 70, "t11",        "BMP/t11.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( T1,      new CDBPlayer( 70, 10, 70, "t12",           "BMP/t12.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( T1,      new CDBPlayer( 70, 10, 70, "t13",           "BMP/t13.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( GZ,      new CDBPlayer( 70, 10, 70, "gz1",          "BMP/gz1.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( GZ,      new CDBPlayer( 70, 10, 70, "gz2",         "BMP/gz2.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( GZ,      new CDBPlayer( 70, 10, 70, "gz3",         "BMP/gz3.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( SOUL,    new CDBPlayer( 70, 10, 70, "soul1",      "BMP/soul1.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( SOUL,    new CDBPlayer( 70, 10, 70, "soul2",      "BMP/soul2.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( SOUL,    new CDBPlayer( 70, 10, 70, "soul3",      "BMP/soul3.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( KHAN,    new CDBPlayer( 70, 10, 70, "khan1",    "BMP/khan1.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( KHAN,    new CDBPlayer( 70, 10, 70, "khan2",    "BMP/khan2.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( KHAN,    new CDBPlayer( 70, 10, 70, "khan3",    "BMP/khan3.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( TOP,     new CDBPlayer( 70, 10, 70, "top1",       "BMP/top1.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( TOP,     new CDBPlayer( 70, 10, 70, "top2",       "BMP/top2.bmp" )  ) );
    m_PlayerList.insert( std::make_pair( TOP,     new CDBPlayer( 70, 10, 70, "top3",       "BMP/top3.bmp" )  ) );

}

void  CPlayerList::GetPlayerList( int TeamName, std::vector<CDBPlayer*> &PlayerNameList  )
{
    std::pair< PlayerList::iterator , PlayerList::iterator > range = m_PlayerList.equal_range( TeamName );

    for( PlayerList::iterator it = range.first; it != range.second; ++it )
        PlayerNameList.push_back( it->second );


}

void CPlayerList::Release()
{
    for( PlayerList::iterator it = m_PlayerList.begin(); it != m_PlayerList.end() ; ++it )
        delete ( (*it).second );

        m_PlayerList.clear();
}
