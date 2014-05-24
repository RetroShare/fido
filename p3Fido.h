/*
    This file is part of the Fido Plugin for Retroshare.

    Fido is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Fido is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Fido.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef P3TORTOISERRS_H
#define P3TORTOISERRS_H


#include "retroshare/rspeers.h"
#include "plugins/rspqiservice.h"

#include <map>


class RsPluginHandler;

class p3Fido : public RsPQIService
{
public:
    p3Fido(RsPluginHandler *pgHandler);

    virtual int tick();

private:
    void pollMaildir();
    void sendMail( const char * filename );

private:
    std::map< std::string, int > m_sentMsgs;
};

#endif // P3TORTOISERRS_H

