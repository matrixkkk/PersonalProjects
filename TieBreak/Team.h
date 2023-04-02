#ifndef   _TEAM_H_
#define  _TEAM_H_

#include <map>
#include <vector>

#include "DBPlayer.h"

class CPlayerList{

    enum TeamArray{ MBC = 0, HANBIT, KTF, T1, GZ, SOUL, KHAN, TOP };

    typedef std::multimap< int, CDBPlayer* >  PlayerList;
    PlayerList m_PlayerList;

public:

    CPlayerList(){ }
    ~CPlayerList(){  Release(); }

    void Init();
    void Release();
    void  GetPlayerList( int TeamName, std::vector<CDBPlayer*> &PlayerNameList  );

};


#endif
