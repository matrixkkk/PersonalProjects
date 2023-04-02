#ifndef  _STAGEPARAMETER_H_
#define _STAGEPARAMETER_H_


#include <deque>
#include <string>

class CStageParameter{

    int                     Team;
    int                     PlayType;
    std::string             MapName;
    std::deque<std::string> m_CharacterOrder;


public:

    void SetTeam        ( int &team )               {   Team     = team; }
    void SetPlayType    ( int type )                {   PlayType = type; }
    void SetMap( const std::string &mapname )       {   MapName = mapname; }
    void SetCharacter(  const std::deque<std::string> &characterorder )  {   m_CharacterOrder = characterorder; }

    int                     GetTeam()       {  return Team;     }
    int                     GetPlaytype()   {  return PlayType; }
    std::string             GetMap()        { return MapName;   }
    std::deque<std::string> GetCharacter()  { return m_CharacterOrder; }

public:

    CStageParameter(){ }
    ~CStageParameter(){ }

};

#endif
