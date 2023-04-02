#ifndef  _DBPLAYER_H_
#define _DBPLAYER_H_
#include <iostream>
#include <cstring>

class CDBPlayer {

    int                 m_Strength;                  //Èû
    int                 m_Speed;                    //½ºÇÇµå
    int                 m_DefenceRange;       //µðÆæ½º ¹üÀ§


    std::string      m_Name;
    std::string      m_bitmapName;           //ºñÆ®¸Ê ÀÌ¸§

public:

    CDBPlayer( int strength, int speed, int defencerange, std::string name,  std::string filename ) :
    m_Strength(strength), m_Speed(speed), m_DefenceRange(defencerange), m_Name( name ), m_bitmapName( filename ){ }
    ~CDBPlayer(){}


    int                 GetStrength             (void) const { return m_Strength;             }
    int                 GetSpeed                (void) const { return m_Speed;                }
    int                 GetDefneceRange   (void) const { return m_DefenceRange;   }
    std::string      GetBitmapName       (void) const { return m_bitmapName;       }
    std::string      GetName                  (void) const{ return m_Name;                  }
};

#endif
