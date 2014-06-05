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

static const char * MY_GXSID = "8f5079e2e2958f1e3b6006fb9cf2b5a5";
static const char * MAILDOMAIN = "ns3.ativel.com";

static const uint16_t RS_SERVICE_TYPE_PLUGIN_FIDO_GW = 0xF1D0;
static const unsigned int TICK_DELAY = 5; // seconds


p3Fido::p3Fido( RsPluginHandler *pgHandler ) :
    RsPQIService( RS_SERVICE_TYPE_PLUGIN_FIDO_GW, TICK_DELAY, pgHandler )
{
}

RsServiceInfo p3Fido::getServiceInfo()
{
    return RsServiceInfo( RS_SERVICE_TYPE_PLUGIN_FIDO_GW, "FIDO", 0, 0, 0, 1 );
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
        mimetic::Mailbox mailbox = (*it).mailbox();

        if( mailbox.domain() != MAILDOMAIN )
            continue;

        numAddr++;

        std::string rsAddr = mailbox.mailbox();
        RsGxsId gxsid( rsAddr );
        mi.rsgxsid_msgto.push_back( RsGxsId( gxsid ) );
    }

    mimetic::AddressList & ccList = me.header().cc();
    for( mimetic::AddressList::const_iterator it = ccList.begin(); it != ccList.end(); it++ ){
        mimetic::Mailbox mailbox = (*it).mailbox();

        if( mailbox.domain() != MAILDOMAIN )
            continue;

        numAddr++;

        std::string rsAddr = mailbox.mailbox();
        RsGxsId gxsid( rsAddr );
        mi.rsgxsid_msgcc.push_back( gxsid );
    }

    m_sentMsgs[ msgId ] = numAddr;

    mi.title = me.header().subject();

    std::string bodyText;
    std::string contentType = me.header().contentType().str();

    if( contentType.find( "text/plain" ) == std::string::npos ){
        mimetic::MimeEntityList& parts = me.body().parts();
        mimetic::MimeEntityList::iterator mbit = parts.begin();
        if( mbit != parts.end() ){
            mimetic::MimeEntity * pme = *mbit;
            std::ostringstream o;
            o << *pme;
            bodyText = o.str();
        }
    }
    else{
        bodyText = me.body();
    }

    mi.msg = bodyText;
    RsGxsId mygxsid( MY_GXSID );
    mi.rsgxsid_srcId = mygxsid;
    mi.msgflags = 0;
    mi.msgId = msgId;

    rsMsgs->MessageSend(mi);
}
