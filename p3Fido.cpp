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

#include "p3Fido.h"
#include "helpers.h"

#include <retroshare/rsmsgs.h>

#include <mimetic/mimetic.h>

#include <QDir>
#include <QStringList>

#include <fstream>
#include <iostream>


static const uint16_t RS_SERVICE_TYPE_FIDO_PLUGIN = 0xBEEE;
static const uint32_t CONFIG_TYPE_FIDO_PLUGIN     = 0xDEADBEEE;

static const char * MAILDOMAIN = "ns3.ativel.com";

p3Fido::p3Fido( RsPluginHandler *pgHandler ) :
    RsPQIService( RS_SERVICE_TYPE_FIDO_PLUGIN, CONFIG_TYPE_FIDO_PLUGIN, 0, pgHandler )
{
}

int p3Fido::tick()
{
    pollMaildir();
    return 0;
}


void p3Fido::pollMaildir()
{
    QString home = getenv( "HOME" );
    QString maildirPath = home + "/Maildir/new";
    QDir maildir( maildirPath );
    if( !maildir.exists() ){
        std::cerr << "Fido: Maildir " << maildirPath.toStdString() << " does not exist!" << std::endl;
        return;
    }
    QStringList files = maildir.entryList();

    for( QStringList::ConstIterator it = files.begin(); it != files.end(); it++ ){
        QString filename = *it;
        if( filename[0] == '.' ) continue;
        QString fullname = maildirPath + '/' + filename;
        std::cerr << "Fido: Maildir entry: " << filename.toStdString() << std::endl;
        sendMail( fullname.toUtf8() );
        remove( fullname.toUtf8() );
    }
}


void p3Fido::sendMail( const char * filename )
{
    std::ifstream mailfile( filename, std::ifstream::in );
    if( !mailfile.good() ){
        std::cerr << "Fido: Cannot open mail file " << filename << std::endl;
        return;
    }
    MessageInfo mi;
    mimetic::MimeEntity me( mailfile );

    std::string msgId = me.header().messageid().str();
    std::map< std::string, int >::iterator msgIt = m_sentMsgs.find( msgId );
    if( msgIt != m_sentMsgs.end() ){ // we had this message already
        (*msgIt).second--;
        if( (*msgIt).second == 1 ){
            m_sentMsgs.erase( msgIt );
        }
        return;
    }


    int numAddr = 0;
    mimetic::AddressList & toList = me.header().to();
    for( mimetic::AddressList::const_iterator it = toList.begin(); it != toList.end(); it++ ){
        std::string to = (*it).str();
        std::vector< std::string > addrParts;
        Fido::split( to, addrParts, '@' );

        if( addrParts[ 1 ] != MAILDOMAIN )
            continue;

        numAddr++;

        std::string rsAddr = addrParts[ 0 ];
        rsAddr = rsAddr.substr( rsAddr.find_first_not_of( " " ) );

        std::string destHash;
        if( rsMsgs->getDistantMessageHash( rsAddr, destHash ) == false ){
            std::cerr << "Fido: Cannot convert address " << rsAddr << " into hash" << std::endl;
            // TODO: bounce message back and return?
        }
        std::cerr << "Fido: Adding to: hash " << destHash << " for addr " << rsAddr << std::endl;
        mi.msgto.push_back( destHash );
    }

    mimetic::AddressList & ccList = me.header().cc();
    for( mimetic::AddressList::const_iterator it = ccList.begin(); it != ccList.end(); it++ ){
        std::string cc = (*it).str();
        std::vector< std::string > addrParts;
        Fido::split( cc, addrParts, '@' );

        if( addrParts[ 1 ] != MAILDOMAIN )
            continue;

        numAddr++;

        std::string rsAddr = addrParts[ 0 ];
        rsAddr = rsAddr.substr( rsAddr.find_first_not_of( " " ) );

        std::string destHash;
        if( rsMsgs->getDistantMessageHash( rsAddr, destHash ) == false ){
            std::cerr << "Fido: Cannot convert address " << rsAddr << " into hash" << std::endl;
            // TODO: bounce message back and return?
        }
        std::cerr << "Fido: Adding cc: hash " << destHash << " for addr " << rsAddr << std::endl;
        mi.msgcc.push_back( destHash );
    }

    m_sentMsgs[ msgId ] = numAddr;


    std::string subject = me.header().subject();
    std::wstring wSubject( subject.begin(), subject.end() );
    mi.title = wSubject;

    mimetic::MimeEntityList& parts = me.body().parts();
    mimetic::MimeEntityList::iterator mbit = parts.begin();
    std::string bodyText;
    if( mbit != parts.end() ){
        mimetic::MimeEntity * pme = *mbit;
        std::ostringstream o;
        o << *pme;
        bodyText = o.str();
    }
    std::wstring wBodyText( bodyText.begin(), bodyText.end() );
    mi.msg = wBodyText;

    rsMsgs->MessageSend(mi);
}
